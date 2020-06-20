# Circular Dependencies Detector

## Presentation:

### Demo Video : https://youtu.be/8ZBd1RLFG3M

**Marketplace link :** https://www.unrealengine.com/marketplace/en-US/slug/circular-dependencies-detector

With the Circular Dependencies Detector plugin, you can quickly detect in your project all circular dependencies, dependencies in both ways. The detection is done in compile and save time. You can place the Editor Widget wherever you want.

Send by mail any suggestion for the plugin. It will always be considered.  
Please report any unexpected behaviours. It will be fixed as soon as possible.

**Support e-mail :** bstt.ue4@gmail.com

## Sources

You can get the latest version of the plugin on this [link](https://github.com/bstt/CircularDependenciesDetector/tree/release).

You can freely **get the plugin via the UE4 marketplace** in order to automatically get the updates.

## Documentation:

### Table of contents

#### 1. [Use Circular Dependencies Detector](#1-Use-Circular-Dependencies-Detector-1)

##### 1.1. [Run the Editor Widget](#11-Run-the-Editor-Widget-1)

##### 1.2. [Display](#12-Display-1)

##### 1.3. [Refresh button](#13-Refresh-button-1)

##### 1.4. [Open asset](#14-Open-asset-1)

##### 1.5. [Exclude button](#15-Exclude-button-1)

#### 2. [Solve circular dependencies](#2-Solve-circular-dependencies-1)

##### 2.1. [Use interfaces](#21-Use-interfaces-1)

##### 2.2. [Use Event Dispatcher](#22-Use-Event-Dispatcher-1)

##### 2.3. [Split Blueprint Function Library](#23-Split-Blueprint-Function-Library-1)

##### 2.4. [Redo the design](#24-Redo-the-design-1)

#### 3. [Contact me](#3-Contact-me-1)

### 1. Use Circular Dependencies Detector

Circular Dependencies Detector is a tool that **only detect** all circular dependencies in a project. You need to **manually solve these circular dependencies**. Some ways to fix them are explained in the section : [Solve circular dependencies](#2-Solve-circular-dependencies-1).

#### 1.1. Run the Editor Widget

For the first opening you need to :

- In Content Browser, enable **Show Plugin Content**,
- Enable **Show Engine Content** (It is recommanded to **_disable Show C++ Classes_**)
- Go to the **Circular Dependencies Detector plugin folder,**
- **Right click** on the CircularDependencies_EWBP (**do not double-click**),
- Select **Run Editor Utility Widget**.

![Show Plugin Content](Documentation/Images/1_11-ShowPluginContent.png)
![Show Engine Content](Documentation/Images/1_12-ShowEngineContent.png)

![Run Editor Utility Widget](Documentation/Images/1_13-RunEditorUtilityWidget.png)

If you opened and closed the EWBP, you can re-open it by going to the **Edit Menu -> Editor Widget -> Run EWBP**.

![Run EWBP](Documentation/Images/1_14-RunEWBP.png)

[Table of contents](#Table-of-contents)

#### 1.2. Display

![Display](Documentation/Images/1_21-Display.png)

Displayed by the plugin :

- number of excluded assets
- all excluded assets
- number of circular dependencies
- all assets involved in circular dependencies grouped by block.

The current display means this :

![Display](Documentation/Images/1_22-DisplayDependencies.png)

_(A -> B means A depends on B)_

If an asset is displayed in 2 blocks it means that it is involved in 2 different circular dependencies.

All diplayed circular dependencies don't contain the excluded assets.

[Table of contents](#Table-of-contents)

#### 1.3. Refresh button

On **compile and save**, UE4 compute the dependencies. Sometimes it is not immediately computed. After a short amount of time (30 seconds maximum), you can press the button refresh to check if the dependencies are still present. Sometimes it is badly computed especially when dependencies were contained in a node group. Close and open again the blueprint to fix the computation of dependencies.

You can check the current dependencies by opening the Reference viewer (Right click on the asset -> Reference viewer).

The plugin is using **only hard referencies**.

[Table of contents](#Table-of-contents)

#### 1.4. Open asset

Press the button of a displayed asset in order to open it. The next asset in the circular dependency is **automatically search**. You just need **to manually make the search result empty** in order to break the circular dependency.

[![Open asset](Documentation/Gifs/Downsized/1_41-OpenAsset.gif)](Documentation/Gifs/1_41-OpenAsset.gif)

**A specific search is done for Blueprint Function Library.**

![Blueprint Function Library Search](Documentation/Images/1_42-BlueprintFunctionLibrarySearch.png)

[Table of contents](#Table-of-contents)

#### 1.5. Exclude button

For each asset involved in a circular dependencies, you can press the **Exclude button** in order to exclude the asset from the search of all circular dependencies.  
For each excluded asset, you can press the **Include button** in order to include the asset in the search of all circular dependencies.

[![Exclude asset](Documentation/Gifs/Downsized/1_51-ExcludeAsset.gif)](Documentation/Gifs/1_51-ExcludeAsset.gif)

[Table of contents](#Table-of-contents)

### 2. Solve circular dependencies

In UE4, it is really simple to generate circular dependencies with Blueprint since all blueprints are in the same scope.
In C++, you must use forward declaration to have circular dependencies, it means that if you have ones, it's intentional.

[Circular dependencies](https://en.wikipedia.org/wiki/Circular_dependency) are considered as an anti-pattern because of their negative effects : tight coupling, memory leaks...  
They can be avoid by using [desing patterns](https://refactoring.guru/design-patterns/catalog).

**Problem :**  
You have 2 classes A and B, **you are forced to make B depends on A** and your class A call some functions funcB1 and funcB2 from class B (and then A depends on B).

![Class B](Documentation/Images/2_1-B.png)
![Class A calling class B](Documentation/Images/2_2-AcallingB.png)

**Current dependencies :**  
![Current dependencies](Documentation/Images/2_3-CurrentDependencies.png)

_(A -> B means A depends on B)_

[Table of contents](#Table-of-contents)

#### 2.1. Use interfaces

[Dependency injection pattern](https://en.wikipedia.org/wiki/Dependency_injection) is a simple way to resolve dependencies problem.

**Solution :**  
Move the functions funcB1 and funcB2 in an interface B_Interface and call the functions from the interface instead of the class B.

![B_Interface](Documentation/Images/2_11-B_Interface.png)
![Class B implementing interface](Documentation/Images/2_12-BimplmentingInterface.png)
![Class A calling B_Interface](Documentation/Images/2_13-AcallingB_Interface.png)

**New dependencies :**  
![Interface dependencies](Documentation/Images/2_14-InterfaceDependencies.png)

**Warning :** You must not directely put a reference of class B or any type that depends on class B. You'll need to put a reference of type B_interface or of a type that depends on B_interface.

![B_Interface with B_Interface ref](Documentation/Images/2_15-B_InterfaceWithBref.png)
![A with B_Interface ref](Documentation/Images/2_16-AwithBref.png)

[Table of contents](#Table-of-contents)

#### 2.2. Use Event Dispatcher

[Observer design pattern](https://refactoring.guru/design-patterns/observer) is another simple way to resolve dependencies problem.

**Solution :**  
Declare 2 events dispatcher within the class A, one of type funcB1, one of type funcB2, subscribe the class B to these dispatcher and call the dispatcher in your class A.

If you need a return value for your 2 functions, you need to create 2 other functions return_funcB1, return_funcB2 within the class A and call them in class B instead of returning values.

![Class B using Event](Documentation/Images/2_21-BusingEvent.png)
![Class A using Event](Documentation/Images/2_22-AusingEvent.png)

**New dependencies :**  
![Event dependencies](Documentation/Images/2_23-EventDependencies.png)

You can even put the object of class A as parameter in order to get the caller (especially needed if there are several callers).

![A with A ref](Documentation/Images/2_24-AwithAref.png)

If you have several behaviours after a return value, you can use an enum parameter in order to switch to the behaviour you want to get.

![Class B with multiple behaviours](Documentation/Images/2_25-BmultipleBehaviours.png)
![Class A with multiple behaviours](Documentation/Images/2_26-AmultipleBehaviours.png)

**Warning :** You can't put a reference of class B or any type that depends on class B.

[Table of contents](#Table-of-contents)

#### 2.3. Split Blueprint Function Library

**Problem :** _(explained with a schema to improve clarity)_

![Library Current Dependencies](Documentation/Images/2_31-LibraryCurrentDependencies.png)

- **MyBlueprint** is a class.
- **MyLib** is a Blueprint Function Library.
- **FuncOfMyBP** is a function of MyLib that depends on MyBlueprint.
- **FuncNotOfMyBP** is a function of MyLib that does **NOT** depend on MyBlueprint.
- MyBlueprint use the function FuncNotOfMyBP.
- MyBlueprint do **NOT** use the function FuncOfMyBP.

_By the way, a function of MyLib that depends on MyBlueprint and that is used by MyBlueprint uses should be inside of MyBlueprint (cf. [Redo the design](#24-Redo-the-design-1))._  
_If you can't move the function inside MyBlueprint, you can still [use interfaces](#21-Use-interfaces-1)._

**Solution :**  
Place the function **FuncNotOfMyBP** in another Blueprint Function Library MyOtherLib.

**New dependencies :**  
![Event dependencies](Documentation/Images/2_32-LibraryDependencies.png)

**Note :** All functions that depends on MyBlueprint must be in a library that is **NOT** used by MyBlueprint or inside MyBlueprint.

[Table of contents](#Table-of-contents)

#### 2.4. Redo the design

If you have circular dependencies, it is certainly a design problem.
This method is the hardest way to solve this but it is also the best.

There is no specific rule or magic trick to solve it : you'll need to use your brain to fix it.

But here are some tips that can help you to make a good design.
Generally all design problem come from a bad data structure, and not from a bad implementation.

You need to wonder :

- Do I really need this ? (Can I get this from another way ?)
- How will I use this ? (Is it displayed ? used for computation ? Is it a storage of a computation ?)
- Do I really need this here ? (Who owns who ?)

It is pretty hard to objectively answer to these questions but it is necessary to improve your data structures.

When you reach the point that you can't improve your data structures anymore, you need to wonder :

- Which object must call this function ? (who interacts with who ?)

[Table of contents](#Table-of-contents)

### 3. Contact me

When you got an error or a crash :

- Save your project.
- Try to reproduce the error.
- Fill this [template e-mail](Template_e_mail.md)
- Send it at this e-mail address : bstt.ue4@gmail.com

Until a valid fix is found, try right-click on content browser and then **Fix up Redirectors in Folder**.  
Please **send the project before** trying this.

[Table of contents](#Table-of-contents)
