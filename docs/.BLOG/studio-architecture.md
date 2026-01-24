# Building Hektor Studio: From C++ to Electron Without Losing Speed

*January 24, 2026*

---

"Just wrap it in Electron."

Every C++ developer has heard this suggestion. Every C++ developer has cringed.

Electron apps are notorious: slow, memory-hungry, battery-draining. VSCode somehow makes it work. Most don't.

So when I decided to build **Hektor Studio** — a visual interface for our C++23 vector database — I had a choice:

1. Build a native Qt/GTK app (fast, ugly, platform-specific)
2. Build a web app (pretty, slow, needs a server)
3. Build an Electron app (pretty, portable, "slow")

I chose option 3. Here's how we made it fast.

## The Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Hektor Studio                          │
├──────────────────────┬──────────────────────────────────────┤
│    Main Process      │         Renderer Process             │
│    (Node.js)         │         (Chromium)                   │
│                      │                                      │
│  ┌────────────────┐  │  ┌──────────────────────────────┐   │
│  │ Native Addon   │  │  │        React UI              │   │
│  │ (N-API/C++)    │◄─┼──┤  - Components                │   │
│  │                │  │  │  - State Management          │   │
│  │ ┌────────────┐ │  │  │  - Visualizations            │   │
│  │ │ BM25       │ │  │  └──────────────────────────────┘   │
│  │ │ Engine     │ │  │                                      │
│  │ ├────────────┤ │  │  ┌──────────────────────────────┐   │
│  │ │ Hybrid     │ │  │  │      IPC Bridge              │   │
│  │ │ Search     │ │  │  │  - Async calls               │   │
│  │ ├────────────┤ │  │  │  - Streaming results         │   │
│  │ │ PQ/HDR     │ │  │  │  - Binary transfer           │   │
│  │ │ Quantizer  │ │  │  └──────────────────────────────┘   │
│  │ ├────────────┤ │  │                                      │
│  │ │ HNSW       │ │  │                                      │
│  │ │ Index      │ │  │                                      │
│  │ └────────────┘ │  │                                      │
│  │       │        │  │                                      │
│  │       ▼        │  │                                      │
│  │ libvdb_core    │  │                                      │
│  └────────────────┘  │                                      │
└──────────────────────┴──────────────────────────────────────┘
```

The key insight: **JavaScript never touches hot paths.**

Every search query, every quantization operation, every index modification — it all happens in native code. The JavaScript layer exists solely for:
- UI rendering (React)
- Event handling
- State management
- IPC coordination

## The Native Addon

### Why Not Just Call the CLI?

We could have done this:

```javascript
const { execSync } = require('child_process');
const result = execSync('hektor search --query "..."');
```

Problems:
- Process spawn overhead (~10ms per call)
- Serialization costs (JSON encode/decode)
- No streaming results
- No shared memory

Instead, we built a **N-API native addon** that links directly against `libvdb_core`.

### The Build Challenge

Here's where it got interesting. Our core library uses C++23:

```cpp
std::expected<SearchResult, Error> search(Query q);
```

Node.js 22 uses V8, which also defines `expected`-like types internally. When we tried to compile:

```
error: ambiguous reference to 'expected'
```

### The Solution: cmake-js

We replaced node-gyp with **cmake-js**, which gives us:
- Full CMake control
- Proper C++23 standard enforcement
- Namespace isolation
- Conditional compilation

Our `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)
project(hektor_addon)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Node.js headers
include_directories(${CMAKE_JS_INC})

# Our addon sources
add_library(${PROJECT_NAME} SHARED
    src/binding.cpp
    src/bm25.cpp
    src/hybrid.cpp
    src/quantization.cpp
    src/index.cpp
)

# Link against our core library
target_link_libraries(${PROJECT_NAME}
    ${CMAKE_JS_LIB}
    vdb_core
)

# Critical: proper prefixes for Node.js
set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)
```

### The N-API Bindings

N-API is Node.js's stable ABI for native addons. Here's a simplified version of our search binding:

```cpp
napi_value Search(napi_env env, napi_callback_info info) {
    // Parse arguments
    size_t argc = 2;
    napi_value argv[2];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    
    // Extract query string
    size_t query_len;
    napi_get_value_string_utf8(env, argv[0], nullptr, 0, &query_len);
    std::string query(query_len, '\0');
    napi_get_value_string_utf8(env, argv[0], query.data(), query_len + 1, nullptr);
    
    // Extract k
    int32_t k;
    napi_get_value_int32(env, argv[1], &k);
    
    // Call native search (THIS IS THE HOT PATH)
    auto results = g_index->search(query, k);
    
    // Convert results to JS array
    napi_value js_results;
    napi_create_array_with_length(env, results.size(), &js_results);
    
    for (size_t i = 0; i < results.size(); i++) {
        napi_value item;
        napi_create_object(env, &item);
        // ... populate result object
        napi_set_element(env, js_results, i, item);
    }
    
    return js_results;
}
```

## Performance Results

We benchmarked three approaches on SIFT-1M (1M vectors, k=10):

| Approach | Latency (p50) | Latency (p99) |
|----------|---------------|---------------|
| CLI subprocess | 15.2ms | 28.4ms |
| REST API | 4.8ms | 12.1ms |
| **Native addon** | **2.9ms** | **4.2ms** |

The native addon is **5x faster** than subprocess calls and **1.7x faster** than REST.

### Memory Efficiency

| Approach | Memory |
|----------|--------|
| Separate CLI | +80MB per call (process overhead) |
| REST server | +120MB (FastAPI + uvicorn) |
| **Native addon** | **+0MB** (shared memory) |

The addon shares memory with the Electron process. No duplication.

## The React Frontend

With performance handled by C++, we could use React freely:

```tsx
// Search component
function SearchPanel() {
    const [query, setQuery] = useState('');
    const [results, setResults] = useState([]);
    const [latency, setLatency] = useState(0);
    
    const handleSearch = async () => {
        const start = performance.now();
        const res = await window.hektor.search(query, 10);
        setLatency(performance.now() - start);
        setResults(res);
    };
    
    return (
        <div className="search-panel">
            <input 
                value={query} 
                onChange={e => setQuery(e.target.value)}
                onKeyDown={e => e.key === 'Enter' && handleSearch()}
            />
            <div className="latency">{latency.toFixed(1)}ms</div>
            <ResultsList results={results} />
        </div>
    );
}
```

### Visualizations

We use D3.js for PQ curve visualization:

```tsx
function PQCurveVisualization({ curve, profile }) {
    const svgRef = useRef();
    
    useEffect(() => {
        const svg = d3.select(svgRef.current);
        
        // Generate curve points
        const points = Array.from({ length: 1000 }, (_, i) => {
            const x = i / 1000;
            const y = applyEOTF(x, curve);
            return [x, y];
        });
        
        // Draw curve
        const line = d3.line()
            .x(d => xScale(d[0]))
            .y(d => yScale(d[1]));
            
        svg.append('path')
            .datum(points)
            .attr('d', line)
            .attr('class', 'pq-curve');
            
    }, [curve, profile]);
    
    return <svg ref={svgRef} />;
}
```

## Lessons Learned

### 1. IPC is the Bottleneck

Electron's IPC (Inter-Process Communication) adds ~0.5ms per call. For a search that takes 2ms, that's 25% overhead.

**Solution**: Batch operations when possible, use binary transfer for large data.

### 2. Don't Fight Electron, Embrace It

Electron gives you Chromium. That means:
- DevTools for debugging
- CSS Grid/Flexbox for layouts
- Web APIs for everything

We use `requestAnimationFrame` for smooth visualizations, `IndexedDB` for local caching, and CSS containment for render performance.

### 3. Native Addons Need Care

Memory leaks in native addons crash the whole app. We use:
- RAII everywhere
- Ref-counting for JS objects
- Explicit cleanup on module unload

## What's Next

Studio 4.1 is functional. Studio 5.0 will be exceptional:

- **WebGPU acceleration** for embedding visualization
- **WASM fallback** when native addon can't compile
- **Plugin system** for custom panels
- **Collaborative features** for teams

## Try It

```bash
cd studio/hektor-app
npm install
npm run build:native
npm run dev
```

You'll see a vector database that doesn't just run fast — it looks good doing it.

---

*Hektor Studio is part of the Hektor 4.1 release.*

— *Ali A. Shakil, ARTIFACT VIRTUAL*
