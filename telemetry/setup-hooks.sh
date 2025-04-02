#!/bin/sh
set -e

HOOK_SOURCE="telemetry/hooks/pre-commit"
HOOK_TARGET=".git/hooks/pre-commit"

echo "🛠️  FS-3 Telemetry: Pre-Commit Hook Setup"

# Does source hook exist?
if [ ! -f "$HOOK_SOURCE" ]; then
  echo "❌ Error: Cannot find hook source at $HOOK_SOURCE"
  exit 1
fi

# Create .git/hooks if needed
if [ ! -d ".git/hooks" ]; then
  echo "📂 Creating .git/hooks directory..."
  mkdir -p .git/hooks
fi

# Symlink the hook
echo "🔗 Linking $HOOK_SOURCE → $HOOK_TARGET"
ln -sf "../../$HOOK_SOURCE" "$HOOK_TARGET"

# Make it executable
echo "🔒 Making hook executable"
chmod +x "$HOOK_TARGET"

# Finish up
echo "✅ Pre-commit hook installed!"
echo "🚦 Try staging a file and running: git commit -m 'Test'"