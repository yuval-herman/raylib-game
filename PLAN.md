# Rewrite

### Namespace pollution

- Raylib has issues with namespace pollution (in windows)
  - We need to separate drawing and logic anyways, so maybe making a draw/paint module is a good idea

### Testing

- While developing I often find myself uncertain whether some functions are bugging out or the parameters just need tuning
  - We need unit testing for functions. At least core ones.

### Data oriented design

- It's best to start designing things to work with DOD now, rather then later

### Separation of concerns

- There should be a thread manager, a UI manager etc...
- Creature should not include drawing functionality, there should be a draw manager separate from inner logic

# Practical steps

1. Make a unit testing framework.

   - Start testing core functions.
   - Add test runner to nob.
   - Add background test runner.

2. Rewrite Creature

   - Focus on DOD design goal.
   - Add unit tests where appropriate.
   - No drawing logic, leave that to draw manager.
