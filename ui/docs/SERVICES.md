# Services

Reference for Angular services in HEKTOR Vector Studio.

## Overview

| Service | Purpose | Location |
|---------|---------|----------|
| `AuthService` | JWT authentication | `services/auth.service.ts` |
| `AgentService` | AI agent with dynamic tools | `services/agent.service.ts` |
| `VectorDbService` | Vector database operations | `services/vector-db.service.ts` |

---

## AuthService

Handles JWT-based authentication with auto-refresh.

### Injection
```typescript
import { AuthService } from './services/auth.service';

class MyComponent {
  auth = inject(AuthService);
}
```

### Signals

| Signal | Type | Description |
|--------|------|-------------|
| `isAuthenticated` | `Signal<boolean>` | Computed auth status |
| `user` | `Signal<User \| null>` | Current user info |
| `isLoading` | `Signal<boolean>` | Loading state |
| `error` | `Signal<string \| null>` | Last error message |

### Methods

| Method | Returns | Description |
|--------|---------|-------------|
| `login(credentials)` | `Promise<boolean>` | Authenticate user |
| `logout()` | `void` | Clear auth state |
| `refreshAccessToken()` | `Promise<boolean>` | Refresh JWT |
| `getToken()` | `string \| null` | Get current token |
| `getAuthHeaders()` | `Record<string, string>` | Get auth headers |
| `hasRole(role)` | `boolean` | Check user role |

### Example
```typescript
async login() {
  const success = await this.auth.login({
    email: 'user@example.com',
    password: 'secret',
    rememberMe: true
  });
  
  if (success) {
    console.log('User:', this.auth.user());
  }
}
```

---

## AgentService

AI agent with dynamic tool registry integration.

### Injection
```typescript
import { AgentService } from './services/agent.service';

class MyComponent {
  agent = inject(AgentService);
}
```

### Signals

| Signal | Type | Description |
|--------|------|-------------|
| `messages` | `Signal<ChatMessage[]>` | Chat history |
| `isThinking` | `Signal<boolean>` | Processing state |

### Methods

| Method | Returns | Description |
|--------|---------|-------------|
| `sendMessage(text)` | `Promise<void>` | Send user message |

### ChatMessage Interface
```typescript
interface ChatMessage {
  role: 'user' | 'model' | 'tool';
  text?: string;
  isAction?: boolean;
  toolName?: string;
  toolArgs?: any;
  toolOutput?: string;
  timestamp: number;
  retrievedTools?: string[];
}
```

### Dynamic Tool Retrieval

The agent uses semantic search to find relevant tools:

```
User Message → Embed → Search system_tools → Inject Tools → Gemini
```

Only matching tools are included in each request, keeping context focused.

---

## VectorDbService

In-memory vector database with Gemini embeddings.

### Injection
```typescript
import { VectorDbService } from './services/vector-db.service';

class MyComponent {
  db = inject(VectorDbService);
}
```

### Signals

| Signal | Type | Description |
|--------|------|-------------|
| `collections` | `Signal<Collection[]>` | All collections |
| `stats` | `Signal<Stats>` | Computed statistics |

### Observables

| Observable | Type | Description |
|------------|------|-------------|
| `telemetry$` | `Subject<TelemetryEvent>` | Latency/throughput events |

### Methods

#### Collection Management

| Method | Returns | Description |
|--------|---------|-------------|
| `createCollection(name, dim?, metric?)` | `Promise<string>` | Create collection |
| `deleteCollection(name)` | `Promise<string>` | Delete collection |
| `updateSchema(name, schema)` | `void` | Update metadata schema |
| `reorderCollections(from, to)` | `void` | Reorder in list |

#### Document Operations

| Method | Returns | Description |
|--------|---------|-------------|
| `ingestData(config)` | `Promise<string>` | Ingest files with chunking |
| `addDocuments(name, docs)` | `Promise<string>` | Add text documents |
| `query(name, params)` | `Promise<VectorDoc[]>` | Semantic search |

#### Tool Registry

| Method | Returns | Description |
|--------|---------|-------------|
| `initSystemTools()` | `Promise<void>` | Initialize tool registry |
| `retrieveTools(query, topK?)` | `Promise<any[]>` | Get matching tools |

#### Utilities

| Method | Returns | Description |
|--------|---------|-------------|
| `seedSampleData()` | `Promise<string>` | Generate sample data |

### Example: Semantic Search
```typescript
const results = await this.db.query('my-collection', {
  query: 'machine learning algorithms',
  topK: 10,
  minScore: 0.7,
  filters: { category: 'AI' }
});

results.forEach(doc => {
  console.log(`${doc.id}: ${doc.score.toFixed(2)}`);
});
```

### Example: Data Ingestion
```typescript
const result = await this.db.ingestData({
  collectionName: 'docs',
  files: [file1, file2],
  chunkSize: 512,
  chunkOverlap: 50,
  strategy: 'recursive'
});
```

---

## Data Types

### Collection
```typescript
interface Collection {
  name: string;
  dimension: number;
  metric: 'cosine' | 'euclidean' | 'dot';
  embeddingModel: string;
  documents: VectorDoc[];
  schema: SchemaField[];
  created_at: number;
  isSystem?: boolean;
}
```

### SearchParams
```typescript
interface SearchParams {
  query: string;
  topK: number;
  minScore: number;
  filters?: Record<string, any>;
}
```

### TelemetryEvent
```typescript
interface TelemetryEvent {
  type: 'latency' | 'throughput';
  value: number;
}
```

---

## Service Relationships

```
┌─────────────────┐
│   AuthService   │
└────────┬────────┘
         │ (protects)
         ▼
┌─────────────────┐     ┌─────────────────┐
│  AgentService   │────▶│ VectorDbService │
└─────────────────┘     └─────────────────┘
    (uses tools)         (stores tools)
```

The `AgentService` depends on `VectorDbService` for:
1. Retrieving dynamic tools from `system_tools`
2. Executing tool operations (create, delete, query, add)
