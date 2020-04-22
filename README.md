# Creative Technologies Project: Creating a system based on a Data-Oriented paradigm that compliments an Object-Oriented Workflow.

This project is designed to allow developers to keep an Object-Oriented workflow whilst still gaining some of the performance benefits from the Data-Oriented paradigm. This is done through leveraging a custom memory allocator for storing related data spatially local and job system to thread permutations on the data.

# Utilising the systems
To implement these systems into your own project just import the following files into your own project and utilise in a similar manner to the ASGEDemo:
 - "JobSystem.hpp"
 - "MemorySystems.hpp"
 - "MemorySystemsInterface.hpp"
 
 # Repo contents:
 This repo contains two projects:
 
  - ASGEDemo - An example program showing off the performance benefits of the systems in the 2D OpenGL game engine [ASGE](https://github.com/HuxyUK/AwesomeSauceGE). If the link to ASGE isn't working the project isn't public yet.
  
  - CreativeTechnologyProject - This is where the original systems were developed along with the multiple test cases refered to within the projects writeups.
  
  
  # ASGEDemo
  This demo was created to show the systems working in a more game related scenario, manipulating many sprites per update. This demo has two main test modes: No Systems and Allman Systems.
  No systems is a simple single threaded and default heap allocated version of the test which acts as a base test case.
  Allman Systems makes use of both the memory allocator and job system in tandem to get better performance out of the same test.
  
  The user navigates the demo through the use of the arrow keys; select options through the enter key and can return to menu through pressing the esc key.
  
  Below are a few of images of the Demo:
  
  ### Menu screen for demo:
  ![](https://i.gyazo.com/ef396a202524f02ceeb3278e78b9d422.png)
  
  ### Selecting how many entities to process in the test:
  ![](https://i.gyazo.com/7e91b98956f1cffc5d06b3990f33bea7.png)
  
  ### The test screen itself:
  ![](https://i.gyazo.com/3d9f08df05ac1153f4cfd4459425f886.png)
  
  # CreativeTechnologyProject
  This is the original visual studio project the library was developed in. This is also where the original benchmarks were made and can still be run to test a more synthetic workload in comparison to its ASGEDemo counterpart.
  
  The ImGui interface is simply interacted with through using the mouse. Which test is being run and its paramaters are all tweaked with ImGui at real time and a graph live updates with the results from the test. The max value of NPCs that can be processed each update through these tests is hard capped at 1,000,000. Any attempt to go higher will reset the value back to 1,000,000.
  
  Below are a few images of the tests and its GUI:
  
  ### No systems test case:
  ![](https://i.gyazo.com/828ee4582d2a5677f7034ec90a902230.png)
  
  ### All Systems test case:
  ![](https://i.gyazo.com/51571df823cf067852500139cfcb7a58.png)
  
  ### Just Job test case:
  ![](https://i.gyazo.com/2cb23f1bb5f276c9954c6ca192ad01a9.png)
  
  ### Just Memory test case:
  ![](https://i.gyazo.com/e3ddd7559abc667c68d6978888e43533.png)
