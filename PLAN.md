# Rewrite

### Namespace pollution

- Raylib has issues with namespace pollution (in windows)
  - We need to separate drawing and logic anyways, so maybe making a draw/paint module is a good idea

### Testing

- While developing I often find myself uncertain whether some functions are bugging out or the parameters just need tuning
  - We need unit testing for functions. At least core ones.

### Data oriented design

- It's best to start designing things to work with DOD now, rather then later

### File/Folder structure

- Two external folders are confusing. Either compile them as libraries as well or rename one of the folders

### Build system

- The build system is a bit messy. No need to rewrite from scratch, but there is a need to extract reused functionality, clean macros and tidy up a bit
