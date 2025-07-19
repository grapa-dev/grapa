### **Deployment Lessons Learned**:
- **Git checkout conflicts** - Use `git clean -fdx` before switching branches
- **Case sensitivity issues** - Remove `docs/site` completely before committing
- **Built site cleanup** - Always use `--clean` flag with mkdocs build
- **Branch switching workflow** - Stash changes, clean workspace, switch, copy, commit, push, return
- **Emergency rollback** - Use `git reset --hard` to previous commit if needed
- **Pre-deployment checklist** - Verify all changes committed, examples tested, build clean
- **Post-deployment verification** - Check site loads, navigation works, examples accessible

## Document Workflow 