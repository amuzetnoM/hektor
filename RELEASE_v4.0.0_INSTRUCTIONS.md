# Instructions for Pushing v4.0.0 Tag

The v4.0.0 tag has been created locally and is ready to be pushed to origin.

## Option 1: Push Tag Directly (Recommended)

Once this PR is merged to main, run:

```bash
git checkout main
git pull origin main
git push origin v4.0.0
```

## Option 2: Create GitHub Release from UI

1. Merge this PR to main
2. Go to: https://github.com/amuzetnoM/hektor/releases/new
3. Choose tag: `v4.0.0` (or create new tag if not available)
4. Target: `main` branch
5. Release title: `v4.0.0 - Security Patch Release`
6. Description: Copy from `docs/changelog/v4.0.0.md`
7. Click "Publish release"

## Verify Tag Locally

To verify the tag was created correctly:

```bash
git tag -l v4.0.0
git show v4.0.0
```

## Release Notes

The complete release notes are available in:
- `docs/changelog/v4.0.0.md`
- Tag annotation (accessible via `git show v4.0.0`)

## What Changed Since v3.0.0

- Security fix: Updated tar dependency from 7.5.2 to 7.5.3
- Fixed vulnerability in absolute linkpath sanitization
- No breaking changes or functional modifications
- Fully backward compatible with v3.0.0

## Next Steps After Publishing

1. Announce the release in project channels
2. Update documentation if needed
3. Monitor for any issues
4. Consider creating Docker images with updated tag
