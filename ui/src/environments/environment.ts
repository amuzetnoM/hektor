/**
 * Environment Configuration
 * 
 * For Gemini API: Set GEMINI_API_KEY in .env file or configure in Settings
 */
export const environment = {
  production: false,

  // API Configuration
  apiUrl: 'http://localhost:8080/api',
  wsUrl: 'ws://localhost:8080/ws',

  // Gemini API - load from localStorage (set via Settings page)
  geminiApiKey: '', // Configured at runtime via Settings

  // Auth Configuration
  tokenKey: 'hektor_auth_token',
  refreshTokenKey: 'hektor_refresh_token',
  tokenExpiryBuffer: 60 * 1000, // Refresh token 1 minute before expiry

  // Feature Flags
  useBackend: false, // Set to true when BFF is ready
  enableMockAuth: true, // Allow mock login for development
};
