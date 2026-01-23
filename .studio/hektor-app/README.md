# HEKTOR Quantization Studio

**Status**: Week 1 Foundation - Project Structure Complete  
**Version**: 1.0.0-alpha  
**Date**: 2026-01-23

## Quick Start

### Installation

```bash
cd .studio/hektor-app
npm install
```

### Development

```bash
npm run dev
```

This starts both the Electron main process and the Vite dev server.

### Building

```bash
npm run build
npm start
```

## Project Structure

```
hektor-app/
├── electron-app/
│   ├── main/              # Main process (Node.js)
│   │   ├── index.ts       # Entry point
│   │   └── tsconfig.json
│   ├── renderer/          # Renderer process (React)
│   │   ├── src/
│   │   │   ├── App.tsx
│   │   │   ├── main.tsx
│   │   │   ├── index.css
│   │   │   └── lib/
│   │   │       └── theme-system.tsx
│   │   └── index.html
│   └── preload/           # Preload scripts
│       └── index.ts
├── themes/                # Theme files (JSON)
│   ├── quantum-slate-dark.json
│   ├── quantum-slate-light.json
│   ├── midnight-purple-dark.json
│   ├── ocean-breeze-dark.json
│   ├── forest-green-dark.json
│   └── sunset-amber-light.json
├── docs/
│   └── THEME_SYSTEM.md    # Theme system documentation
├── package.json
├── tsconfig.json
├── vite.config.ts
├── tailwind.config.js
└── postcss.config.js
```

## Features Implemented

### ✅ Week 1 Foundation

- [x] Project structure setup
- [x] Electron + React + TypeScript configuration
- [x] Vite build system
- [x] IPC communication setup
- [x] **Theme system with plugin architecture**
- [x] **6 built-in themes (3 dark, 3 light)**
- [x] Theme selector UI
- [x] CSS variables for theming
- [x] Hot reload for development

### 🎨 Theme System

The theme system is designed to be:
- **External**: Themes don't modify core UI code
- **Extensible**: Users can add custom themes
- **Hot-swappable**: Change themes without restart
- **Dev-friendly**: JSON-based, easy to create

See [docs/THEME_SYSTEM.md](docs/THEME_SYSTEM.md) for details.

## Next Steps (Week 2-3)

- [ ] Build C++ native addon
- [ ] Setup CMake integration
- [ ] Create N-API bindings
- [ ] Test zero-copy data transfer
- [ ] Add Three.js for 3D rendering
- [ ] Create viewport component

## Scripts

- `npm run dev` - Start development environment
- `npm run build` - Build for production
- `npm start` - Start built application
- `npm run package` - Package for distribution
- `npm run lint` - Lint code
- `npm run format` - Format code with Prettier

## Technology Stack

- **Electron**: 28.x
- **React**: 18.x
- **TypeScript**: 5.3.x
- **Vite**: 5.x
- **Tailwind CSS**: 3.x
- **Three.js**: 0.160.x (ready to integrate)

## Documentation

- [Theme System](docs/THEME_SYSTEM.md)
- [Design Specifications](../../ELECTRON_UI_DESIGN.md)
- [Build Documentation](../../ELECTRON_UI_BUILD.md)
- [Implementation Checklist](../../ELECTRON_UI_CHECKLIST.md)

## Contributing

Follow the specifications in the root documentation:
1. Read `ELECTRON_UI_DESIGN.md` for design guidelines
2. Follow `ELECTRON_UI_BUILD.md` for build procedures
3. Track progress with `ELECTRON_UI_CHECKLIST.md`

## License

MIT
