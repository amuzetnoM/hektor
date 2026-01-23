import React from 'react';
import { useTheme } from './lib/theme-system';

function App() {
  const { theme, setTheme, availableThemes } = useTheme();

  return (
    <div className="app-container" data-theme={theme.id}>
      <header className="app-header">
        <h1 className="app-title">HEKTOR Quantization Studio</h1>
        <div className="theme-selector">
          <select 
            value={theme.id} 
            onChange={(e) => setTheme(e.target.value)}
            className="theme-dropdown"
          >
            {availableThemes.map(t => (
              <option key={t.id} value={t.id}>
                {t.name}
              </option>
            ))}
          </select>
        </div>
      </header>
      
      <main className="app-main">
        <div className="welcome-section">
          <h2>Welcome to HEKTOR</h2>
          <p className="subtitle">
            Industry-first Perceptual Quantization Studio for 3D Vector Visualization
          </p>
          <div className="feature-grid">
            <div className="feature-card">
              <h3>3D Visualization</h3>
              <p>Multi-geometry support: Euclidean, Hyperbolic, Parabolic</p>
            </div>
            <div className="feature-card">
              <h3>Perceptual Quantization</h3>
              <p>HDR-aware quantization with real-time preview</p>
            </div>
            <div className="feature-card">
              <h3>Native Performance</h3>
              <p>Direct C++ integration via N-API for sub-ms latency</p>
            </div>
            <div className="feature-card">
              <h3>AI Assistant</h3>
              <p>Contextual suggestions and query optimization</p>
            </div>
          </div>
        </div>
      </main>
      
      <footer className="app-footer">
        <p>HEKTOR v4.0.0 | Quantum Slate Theme Active</p>
      </footer>
    </div>
  );
}

export default App;
