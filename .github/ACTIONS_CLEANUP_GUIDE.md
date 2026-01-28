# GitHub Actions Cleanup Guide

## Current Status

As of 2026-01-28, there are **924 workflow runs** in the Actions panel, with many showing "action_required" status due to CodeQL security alerts.

## How to Clean Up Actions Panel

### Option 1: Manual Cleanup (Recommended)

1. **Navigate to Actions Tab**
   - Go to https://github.com/amuzetnoM/hektor/actions

2. **Delete Old Workflow Runs**
   - Click on any workflow in the left sidebar
   - For each old/failed run:
     - Click the run
     - Click the `...` (three dots) menu in the top right
     - Select "Delete workflow run"
   
3. **Bulk Delete Using GitHub CLI** (Fastest)
   ```bash
   # Install GitHub CLI if needed
   # brew install gh (macOS)
   # Or download from https://cli.github.com/
   
   # Authenticate
   gh auth login
   
   # Delete all failed workflow runs
   gh run list --repo amuzetnoM/hektor --status failure --limit 100 --json databaseId -q '.[].databaseId' | \
     xargs -I {} gh run delete {} --repo amuzetnoM/hektor
   
   # Delete all action_required runs (older than 7 days)
   gh run list --repo amuzetnoM/hektor --status action_required --limit 100 --json databaseId -q '.[].databaseId' | \
     xargs -I {} gh run delete {} --repo amuzetnoM/hektor
   
   # Delete old successful runs (keep last 30)
   gh run list --repo amuzetnoM/hektor --status success --limit 500 --json databaseId -q '.[].databaseId' | \
     tail -n +31 | xargs -I {} gh run delete {} --repo amuzetnoM/hektor
   ```

### Option 2: Disable and Re-enable Workflows

If you want a fresh start:

1. Go to Settings → Actions → General
2. Disable Actions temporarily
3. Wait a moment, then re-enable
4. Old runs will remain but new ones will start fresh

## Preventing Future Clutter

### 1. Configure Workflow Retention

Add to repository settings or each workflow:

```yaml
# In each workflow file, add:
on:
  # ... your triggers ...

# Limit workflow run retention
env:
  ACTIONS_RETENTION_DAYS: 30  # Keep runs for 30 days only
```

### 2. Set Repository-Level Retention

1. Go to Settings → Actions → General
2. Scroll to "Artifact and log retention"
3. Set to 30 days (or less)

### 3. Use Conditional Workflow Runs

The workflows are already optimized with:
- Branch filters (main, develop, copilot/**)
- Path filters where appropriate
- Proper triggers (push, pull_request, workflow_dispatch)

### 4. Regular Maintenance Schedule

Set a monthly reminder to:
- Review and delete old workflow runs
- Check for failed runs that need attention
- Update workflow configurations as needed

## Understanding "action_required" Status

This status appears when:
1. **CodeQL finds security issues** - These need to be reviewed in the Security tab
2. **Dependabot alerts** - These need dependency updates
3. **Required checks fail** - These need fixes before merging

To resolve:
1. Go to Security tab → Code scanning alerts
2. Review each alert
3. Either fix the issue or dismiss if false positive
4. The workflow status will update automatically

## Current Workflow Status

Active workflows:
- ✅ `ci.yml` - Main CI pipeline (builds, tests, linting)
- ✅ `codeql.yml` - Security scanning
- ✅ `docker-publish.yml` - Docker image builds
- ✅ `build-linux.yml` - Linux wheel builds
- ✅ `release.yml` - Release automation

All workflows are properly configured and should run cleanly after the security fixes in PR #38.

## Quick Commands Reference

```bash
# List recent runs
gh run list --repo amuzetnoM/hektor --limit 10

# View specific run
gh run view <run-id> --repo amuzetnoM/hektor

# Download logs
gh run download <run-id> --repo amuzetnoM/hektor

# Re-run failed jobs
gh run rerun <run-id> --repo amuzetnoM/hektor

# Watch a running workflow
gh run watch <run-id> --repo amuzetnoM/hektor
```

## After This PR Merges

Once PR #38 is merged:
1. All security vulnerabilities will be fixed
2. New workflow runs should complete successfully
3. CodeQL should report fewer/no issues
4. Actions panel will stabilize with clean runs

You can then clean up the old failed runs using the methods above.
