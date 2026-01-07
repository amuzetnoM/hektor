# Components

Reference for all Angular components in HEKTOR Vector Studio.

## Overview

| Component | Purpose | Location |
|-----------|---------|----------|
| `AppComponent` | Main layout with auth gate | `app.component.ts` |
| `LoginComponent` | Authentication UI | `components/login.component.ts` |
| `ChatWidgetComponent` | AI Agent chat sidebar | `components/chat-sidebar.component.ts` |
| `ProjectionViewComponent` | 2D/3D visualization | `components/projection-view.component.ts` |
| `HealthMonitorComponent` | Telemetry charts | `components/health-monitor.component.ts` |
| `IngestionWizardComponent` | File upload wizard | `components/ingestion-wizard.component.ts` |
| `PlaygroundComponent` | Query testing | `components/playground.component.ts` |
| `SchemaBuilderComponent` | Metadata schema editor | `components/schema-builder.component.ts` |

---

## LoginComponent

Modern login form with gradient styling and dev mode support.

### Selector
```html
<app-login />
```

### Features
- Email/password validation
- Password visibility toggle
- "Remember me" option
- Loading state during authentication
- Error display with animation
- Dev mode indicator

### Dependencies
- `AuthService`
- `FormsModule`

---

## ChatWidgetComponent

Floating AI chat sidebar with tool execution display.

### Selector
```html
<app-chat-widget />
```

### Features
- Expandable sidebar panel
- Message history with roles (user, model, tool)
- Tool execution visualization
- Retrieved tools display
- Thinking indicator

### Public Methods
- `toggle()` - Toggle sidebar visibility

### Dependencies
- `AgentService`

---

## ProjectionViewComponent

Interactive 2D/3D vector space visualization.

### Selector
```html
<app-projection-view [documents]="documents" />
```

### Inputs
| Input | Type | Description |
|-------|------|-------------|
| `documents` | `VectorDoc[]` | Documents to visualize |

### Features
- **2D Mode**: D3.js scatter plot with zoom/pan
- **3D Mode**: Three.js point cloud with orbital controls
- Mode toggle (2D ↔ 3D)
- Document selection on click
- Color-coded clusters
- Auto-rotation in 3D

### Performance
- ~50,000 points at 60 FPS in 3D mode
- Uses `THREE.Points` with `BufferGeometry`

---

## HealthMonitorComponent

Dashboard with real-time telemetry charts.

### Selector
```html
<app-health-monitor />
```

### Features
- Latency chart (line graph)
- Throughput chart (line graph)
- Collection statistics cards
- Memory usage display
- Live updates via RxJS

### Data Source
- `VectorDbService.telemetry$` observable
- `VectorDbService.stats` computed signal

---

## IngestionWizardComponent

Modal wizard for data ingestion.

### Selector
```html
<app-ingestion-wizard 
  [collectionName]="name"
  (close)="onClose()"
  (ingest)="onIngest($event)"
/>
```

### Inputs
| Input | Type | Description |
|-------|------|-------------|
| `collectionName` | `string` | Target collection |

### Outputs
| Output | Type | Description |
|--------|------|-------------|
| `close` | `void` | Modal close request |
| `ingest` | `IngestionConfig` | Ingestion configuration |

### Features
- Drag-and-drop file upload
- File list management
- Chunk size configuration
- Chunk overlap configuration
- Split strategy selection (recursive, fixed, semantic)

---

## PlaygroundComponent

Query testing interface for semantic search.

### Selector
```html
<app-playground [collectionName]="name" />
```

### Inputs
| Input | Type | Description |
|-------|------|-------------|
| `collectionName` | `string` | Collection to query |

### Features
- Query input
- Top-K slider
- Min score threshold
- Results display with scores
- Query execution timing

---

## SchemaBuilderComponent

Dynamic metadata schema editor.

### Selector
```html
<app-schema-builder 
  [schema]="fields"
  (save)="onSave($event)"
/>
```

### Inputs
| Input | Type | Description |
|-------|------|-------------|
| `schema` | `SchemaField[]` | Current schema |

### Outputs
| Output | Type | Description |
|--------|------|-------------|
| `save` | `SchemaField[]` | Updated schema |

### Features
- Add/remove fields
- Field name editing
- Type selection (string, number, boolean, date, list)
- Required flag toggle

---

## Data Types

### VectorDoc
```typescript
interface VectorDoc {
  id: string;
  content: string;
  metadata: Record<string, any>;
  vector?: number[];
  projection: [number, number];
  projection3d: [number, number, number];
  cluster?: number;
  score?: number;
}
```

### IngestionConfig
```typescript
interface IngestionConfig {
  collectionName: string;
  files: File[];
  chunkSize: number;
  chunkOverlap: number;
  strategy: 'recursive' | 'fixed' | 'semantic';
}
```

### SchemaField
```typescript
interface SchemaField {
  name: string;
  type: 'string' | 'number' | 'boolean' | 'date' | 'list';
  required: boolean;
}
```
