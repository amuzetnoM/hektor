import { contextBridge, ipcRenderer } from 'electron';

// Expose protected methods that allow the renderer process to use
// the ipcRenderer without exposing the entire object
contextBridge.exposeInMainWorld('electronAPI', {
  getAppVersion: () => ipcRenderer.invoke('get-app-version'),
  getTheme: (themeName: string) => ipcRenderer.invoke('get-theme', themeName),
  onThemeChanged: (callback: (theme: any) => void) => {
    ipcRenderer.on('theme-changed', (_, theme) => callback(theme));
  }
});

// Theme API for external theme system
contextBridge.exposeInMainWorld('themeAPI', {
  loadTheme: (themePath: string) => ipcRenderer.invoke('load-theme', themePath),
  getAvailableThemes: () => ipcRenderer.invoke('get-available-themes'),
  setActiveTheme: (themeId: string) => ipcRenderer.invoke('set-active-theme', themeId)
});
