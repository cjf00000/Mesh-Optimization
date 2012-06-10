【使用方法】
Usage: 
mesh_animate input                			        Create an animation to compare the algorithms. 
mesh_show mode input percentage           			Reduce input model to percentage, show the image.
mesh_model mode input output percentage				Output the reduced model to output.

Mode Parameters:
        l						        use edge length as cost function. v=(v1+v2)/2.
        q							use the quadratic cost function. v=(v1+v2)/2.
        qopt     						use the quadratic cost function. v is optimal.

Examples:
mesh_animate sphere.obj						Create an animation of reducing sphere.obj from the original to zero faces.
mesh_show q sphere.obj 20					Show the result of reducing sphere.obj to 20% faces.	
mesh_model qout sphere.obj sphere_opt.obj 20			Output the result of reducing sphere.obj to 20% faces to sphere_opt.obj

【界面说明】
动画模式：
  |	左	右
--+---------------------------
上|	原始	边长度
下|	二次	二次（最优v）

显示模式：
使用↑←↓→方向键旋转视角。

注意：
程序启动和处理需要若干时间，这段时间内看不到程序窗口，请耐心等待。

【声明】
OpenGL显示部分，使用了Jeff Molofee (NeHe)的OpenGL示例程序中的代码
http://nehe.gamedev.net/tutorial/3d_shapes/10035/

【编译】
请使用Visual Studio 2010编译工程。

Project 文件夹中包含：

│  MeshOptimizing.sln			VS2010解决方案
│  
└─MeshOptimizing
        animate.cpp			动画主程序
        LinearEquation_Special.cpp	求解线性方程组
        LinearEquation_Special.h
        Mesh.cpp			网格简化主程序
        Mesh.h
        MeshOptimizing.vcxprojVS2010工程
        MeshOptimizing.vcxproj.filters
        MeshOptimizing.vcxproj.user
        model.cpp			模型输出主程序
        show.cpp			显示静态模型主程序
        SimpleObject.cpp		.Obj Parser
        SimpleObject.h
        Vec3f.cpp
        Vec3f.h
        
