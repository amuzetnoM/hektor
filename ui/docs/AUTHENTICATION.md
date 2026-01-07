# Authentication

HEKTOR implements JWT-based authentication with support for both development (mock) and production modes.

## Overview

| Feature | Description |
|---------|-------------|
| Token Storage | localStorage |
| Token Format | JWT (JSON Web Token) |
| Auto Refresh | Before expiry (configurable buffer) |
| Role Support | Via JWT payload |

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        AuthService                               │
├─────────────────────────────────────────────────────────────────┤
│  Signals                                                          │
│  ├── _token: string | null                                        │
│  ├── _user: User | null                                           │
│  └── isAuthenticated: computed<boolean>                           │
├─────────────────────────────────────────────────────────────────┤
│  Methods                                                          │
│  ├── login(credentials) → Promise<boolean>                        │
│  ├── logout() → void                                              │
│  ├── refreshAccessToken() → Promise<boolean>                      │
│  ├── getToken() → string | null                                   │
│  └── getAuthHeaders() → Record<string, string>                    │
└─────────────────────────────────────────────────────────────────┘
```

## Configuration

### Environment Settings

```typescript
// src/environments/environment.ts
export const environment = {
  // Token storage keys
  tokenKey: 'hektor_auth_token',
  refreshTokenKey: 'hektor_refresh_token',
  
  // Refresh buffer (ms before expiry)
  tokenExpiryBuffer: 60 * 1000,
  
  // Feature flags
  enableMockAuth: true,  // Set false for production
};
```

## Development Mode

When `enableMockAuth: true`, the system uses mock authentication:

### Valid Credentials
- **Email**: Any string containing `@` (e.g., `test@example.com`)
- **Password**: Any string with 4+ characters

### Role Assignment
- Include `admin` in email for admin role: `admin@company.com`
- All other emails get `user` role

### Mock JWT Structure
```json
{
  "sub": "user_1704672000000",
  "email": "test@example.com",
  "name": "test",
  "role": "user",
  "iat": 1704672000,
  "exp": 1704758400,
  "iss": "hektor-dev",
  "aud": "hektor-ui"
}
```

> ⚠️ **Warning**: Mock JWTs are NOT cryptographically signed and should NEVER be used in production.

## Production Mode

### Backend Requirements

Implement these endpoints:

#### POST /auth/login
```json
// Request
{
  "email": "user@example.com",
  "password": "secret"
}

// Response (200 OK)
{
  "token": "eyJhbGci...",
  "refreshToken": "eyJhbGci..."
}

// Response (401 Unauthorized)
{
  "message": "Invalid credentials"
}
```

#### POST /auth/logout
```http
Authorization: Bearer <token>

// Response (200 OK)
{}
```

#### POST /auth/refresh
```json
// Request
{
  "refreshToken": "eyJhbGci..."
}

// Response (200 OK)
{
  "token": "eyJhbGci...",
  "refreshToken": "eyJhbGci..." // Optional
}
```

### JWT Payload Structure

```typescript
interface JwtPayload {
  sub: string;           // User ID
  email?: string;
  name?: string;
  role?: string;
  permissions?: string[];
  iat: number;           // Issued at
  exp: number;           // Expiration
  iss?: string;          // Issuer
  aud?: string;          // Audience
}
```

## HTTP Interceptor

All API requests automatically include the Authorization header:

```typescript
// auth.interceptor.ts
Authorization: Bearer <token>
```

### Excluded Endpoints
- `/auth/login`
- `/auth/register`
- `/auth/refresh`
- `/health`

### Error Handling
- **401 Unauthorized**: Triggers logout
- **403 Forbidden**: Logged (insufficient permissions)

## Route Guards

### authGuard
Protects routes from unauthenticated access:

```typescript
// Usage in routes
{
  path: 'dashboard',
  component: DashboardComponent,
  canActivate: [authGuard]
}
```

### roleGuard
Protects routes based on user role:

```typescript
{
  path: 'admin',
  component: AdminComponent,
  canActivate: [roleGuard],
  data: { roles: ['admin'] }
}
```

### guestGuard
Only allows unauthenticated users (e.g., login page):

```typescript
{
  path: 'login',
  component: LoginComponent,
  canActivate: [guestGuard]
}
```

## JWT Utilities

Available in `src/utils/jwt.utils.ts`:

| Function | Description |
|----------|-------------|
| `decodeJwt(token)` | Decode JWT payload (no verification) |
| `isTokenExpired(token, buffer?)` | Check if token is expired |
| `getTokenExpirationDate(token)` | Get expiry as Date object |
| `getTimeUntilExpiry(token)` | Get ms until expiry |
| `getUserFromToken(token)` | Extract user info |

## Security Best Practices

1. **Never trust client-side validation** - Always verify JWT on backend
2. **Use HTTPS** in production
3. **Keep tokens short-lived** - Recommended: 15-60 minutes
4. **Implement token rotation** - Issue new refresh token on each refresh
5. **Consider HttpOnly cookies** for enhanced XSS protection
6. **Validate issuer and audience** claims on backend
