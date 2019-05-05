| 课程名称 | 计算机图形学 | 任课老师 | 高成英 |
| :------------: | :-------------: | :------------: | :-------------: |
| 年级 | 16级 | 专业（方向） | 软件工程（数字媒体）|
| 学号 | 16340255 | 姓名 | 谢涛 |
| 电话 | 13670828568 | Email | 1176748429@qq.com |
| 开始日期 | 2019-03-01 | 完成日期 | 2019-03-04 |

## Basics of CG

### 1、谈谈自己对计算机图形学的理解
在看完有关资料尤其是视频后，觉得计算机图形学是一门很有意思的学科，且可视化的成果也使研究过程中较有成就感，不会那么枯燥。它以数学知识为核心支撑，利用计算机成像技术和编程使得可以在计算机上对二维和三维图形进行建模、渲染、可视化显示（静态或动态），逼真还原真实世界中物体的形态、运动规律、光照规律等细节，达到给人以真实感的目的，甚至可以进行人机交互。计算机图形学前景广阔，可应用于诸多领域，如VR、AR。随着这些领域的发展，计算机图形学的重要性也日益体现，同时也面临着诸多挑战，需要不断地深入研究和突破。

### 2、回答什么是OpenGL? OpenGL ES? Web GL? Vulkan? DirectX? 
- [OpenGL](https://baike.baidu.com/item/OpenGL/238984?fr=aladdin)。全名Open Graphics Library，是一个功能强大，调用方便的开放的底层图形库，用于渲染二维或三维矢量图形的跨语言、跨平台的应用程序编程接口（API）。这个接口由近350个不同的函数调用组成，用来从简单的图形比特绘制复杂的三维景象。OpenGL常用于计算机辅助设计（CAD）、虚拟实境、科学可视化程序和电子游戏开发。
- [OpenGL ES](https://baike.baidu.com/item/OpenGL%20ES/7922580?fr=aladdin)。OpenGL ES是OpenGL三维图形API的子集，专用于手机、PDA和游戏设备等嵌入式系统（Embedded Systems）。相对OpenGL，它去除了四边形、多边形等复杂图元等和一些非绝对必要的特性。
- [WebGL](https://baike.baidu.com/item/WebGL/592485?fr=aladdin)。全名Web Graphics Library，是一种3D绘图协议，这种绘图技术标准为OpenGL ES 2.0提供JavaScript支持，并借助系统显卡，使得WebGL可以为网页端的HTML5 Canvas提供硬件3D加速渲染。WebGL技术标准免去了使用开发网页专用渲染插件的麻烦，可被用于创建具有复杂3D结构的网站页面，甚至可以用来设计3D网页游戏等等。
- [Vulkan](https://baike.baidu.com/item/Vulkan/17543632?fr=aladdin)。也是一个跨平台的2D和3D绘图应用程序接口（API）。相对于OpenGL，Vulkan大幅降低了CPU在提供重要特性、性能和影像质量时的“API 开销”（CPU在分析游戏的硬件需求时所执行的后台工作），而且可以使用通常通过OpenGL无法访问的GPU硬件特性。
- [DirectX](https://baike.baidu.com/item/DirectX/314119?fr=aladdin)。全名Direct eXtension，是由微软公司创建的多媒体编程接口，旨在使基于Windows的计算机成为运行和显示具有丰富多媒体元素（例如全色图形、视频、3D 动画和丰富音频）的应用程序的理想平台。DirectX包括安全和性能更新程序，以及许多涵盖所有技术的新功能，应用程序可以通过使用DirectX API来访问这些新功能，因此被广泛使用于Microsoft Windows、Microsoft XBOX系列电子游戏开发，但是只支持这些Microsoft平台。

### 3、gl.h glu.h glew.h 的作用分别是什么？
- gl.h。是OpenGL 1.1的一个基础头文件，包含了OpenGL所使用的函数和常量声明。如果要使用高于版本1.1的特性，则需要添加额外的头文件进行扩展。
- glu.h。u是utilities，GLU即OpenGL实用库，包含了一些方便的方法，但是已经很久没有更新了，有些过时。
- glew.h。由于单纯使用gl.h需要自己检查版本及添加扩展，较繁琐，便有了glew来自动完成这些繁琐工作。且glew.h包含了gl、glu、glext、wgl、glx的全部函数，因此只需要包含一个glew.h就能使用目前的常用接口。
- [其他](https://stackoverflow.com/questions/12122631/difference-between-opengl-files-glew-h-and-gl-h-glu-h)
  

### 4、使用GLFW和freeglut的目的是什么？
- GLFW是一个专门针对OpenGL的C语言库、框架，它提供了一些渲染物体所需的最低限度的接口。它允许用户创建OpenGL上下文，定义窗口参数以及处理用户输入。相比freeglut，GLFW更多应用于游戏开发。
- GLUT很久没有更新，过时且需要改善，freeglut应运而生。freeglut就是GLUT的一个完全开源替代库，它还提供了一些glut所没有的功能，并改善了glut的一些不方便之处。

### 5、选择一个SIGGRAPH 2017/2018上你最喜欢的专题，介绍该专题是做什么的，使用了什么CG技术？（不少于100字）
浏览了一遍SIGGRAPH 2017/2018上各个专题，都挺喜欢，尤其是流体模拟，Fluid足足占了三个板块（Fluid Control & Synthesis、Fluid II、Fluid III），且其他专题也略有涉及。该专题主要是研究如何在各种场景下（不同碰撞角度、不同材质、不同密度的流体等），逼真地模拟出流体应有的形态、运动及之间的作用力。关于使用的CG技术，由于课程尚未深入，不太了解，但必然离不开数学、物理等基础学科，以便建立物体的运动方程模型。一个大致的流程：建模=>渲染=>动画=>（人机交互）。流体里面要着重考虑的特性是流体无法保持原来形状，像液体和气体，因此可能需要将流体分解成许多微小的细粒度物体，微粒各自进行运动模拟，同时考虑微粒之间的碰撞、融合等其他相互作用，或者是看成连续的介质通过某些连续力学理论进行求解。比如让我印象较为深刻的两篇，第一篇Multi-species simulation of porous sand and water mixtures就是像前面提到的将水和沙子都离散化成粒子，分别计算各自的运动，同时计算两者之间的拖拽力；第二篇Anisotropic Elastoplasticity for Cloth, Knit and Hair Frictional Contact，则将毛发和布料看成雪、沙子、水之类的连续物质，然后用连续介质力学建立物理模型，将碰撞和摩擦放到物质点的框架中去求解。这两篇论文的视频都能让人感受到CG的魅力所在。