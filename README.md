## Build & Tests
### Method 1: Open CMake project in Visual Studio, build and run tests from Test Explorer

### Method 2: Build with CMake in terminal
**Build:**
```bash
mkdir build
cd build
cmake ..
make
```

**Run Tests:**
```
ctest
```
**Note: The solution was tested on Windows 11 and Ubuntu(WSL2)**

## Reasoning
The solution assumes that the consumers always start from index 0, since they have no identifier and their progression on the history cannot be tracked. This assumption was made due to my inability to figure out a way to make the consumers end up in the same final state while also allowing me to significantly prune the command history. Another assumption the solution makes is that the consumers can handle a delete command for an object that does not exist. This is required because the solution keeps the last delete for an object even though it technically might be redundant. By doing this we prevent an object in a consumer that lags behind to skip a delete and remain with an outdated state, but open up the possibility for delete duplication in lagging consumers which we assume they can handle.

Benefit Example:
 1. createObject(1)
 2. Consumer 1 joins and reads the create command
 3. deleteObject(1) - this prunes the create command
 4. createObject(1) - here the delete becomes redundant but we keep it
 5. Consumer 1 continues and first correctly deletes object 1 and then creates it anew without keeping any old state

Drawback Example:
1. createObject(1)
2. deleteObject(1)
3. Consumer 1 joins and reads delete command
4. createObject(1)
5. deleteObject(1) - this prunes both the create and the previous delete command
6. Consumer 1 rejoins but since the second createObject command is gone it directly reads a second deleteCommand

Other than these assumptions the solution works based on a hash map holding the latest state for each object and a red-black BST "holding" the commands that were executed at each index.
At each new command we use the state hash map to quickly get access to the state of a given object and check for any previous commands that have become redundant. If such exist we can quickly delete them from our command history. Afterwards when getCommand is called we try to get the command at index startIndex or the closest next using the upper_bound property of the BST and get all the meaningful commands in the range specified by the consumer. Any gaps that apper we take for NoOp commands we can ignore in execution, but count towards the number of processed commands in order to keep the illusion in the consumers that the Producer is running a continious command stream.

**Bonus:** As a bonus memory optimization all property name strings are kept in a central repository which provides the state hash map and all setProperty commands with a pointer to them allowing for less space consumption in big strings and reusability if many objects share the same properties.
