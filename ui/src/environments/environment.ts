/**
 * Environment Configuration
 * 
 * Production: Set these via environment variables or build-time replacement
 */
export const environment = {
  production: false,

  // API Configuration
  apiUrl: 'http://localhost:8080/api',
  wsUrl: 'ws://localhost:8080/ws',

  // Auth Configuration
  tokenKey: 'hektor_auth_token',
  refreshTokenKey: 'hektor_refresh_token',
  tokenExpiryBuffer: 60 * 1000, // Refresh token 1 minute before expiry

  // Feature Flags
  useBackend: false, // Set to true when BFF is ready
  enableMockAuth: true, // Allow mock login for development
};

