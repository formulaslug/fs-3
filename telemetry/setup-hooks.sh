# telemetry/setup-hooks.sh
#!/bin/bash
ln -sf ../../telemetry/hooks/pre-commit .git/hooks/pre-commit
chmod +x .git/hooks/pre-commit