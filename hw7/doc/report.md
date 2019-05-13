| 课程名称 | 计算机图形学 | 任课老师 | 高成英 |
| :------------: | :-------------: | :------------: | :-------------: |
| 年级 | 16级 | 专业（方向） | 软件工程（数字媒体）|
| 学号 | 16340255 | 姓名 | 谢涛 |
| 电话 | 13670828568 | Email | 1176748429@qq.com |
| 开始日期 | 2019-05-11 | 完成日期 | 2019-05-13 |

[TOC]

## Basic
1. 实现方向光源的Shadowing Mapping: 
    - 要求场景中至少有一个object和一块平面(用于显示shadow)，光源的投影方式任选其一即可
    - 在报告里结合代码，解释Shadowing Mapping算法 
2. 修改GUI 

## Bonus

## 作业要求 
1. 把运行结果截图贴到报告里，并回答作业里提出的问题。 
2. 报告里简要说明实现思路，以及主要function/algorithm的解释。 
3. 虽然learnopengl教程网站有很多现成的代码，但是希望大家全部手打，而不是直接copy。

## 效果
![](img/shadow.gif)

## Shadowing Mapping算法解释
Shadowing Mapping 的主要思想是：**以光源的视角进行渲染，能看到的东西都点亮，看不见的东西就在阴影里面**。

### 使用深度缓冲的原因
从光源出发能否看见一个物体，在数学中的衡量方法通常是使用射线，如果从点光源发出的射线能够与物体产生碰撞，那么就代表能看见该物体（能点亮）。同时射线还可以用来判断某个点是否处在阴影之中，即如果射线第一次击中的物体的 “最近点” 比射线上其他点更近，是的话那么其他点就在阴影中。然而对光源发出的射线上所有的点进行遍历判断是个效率极其低下的操作，况且还不止一条射线，这在实时渲染的应用中根本无法容忍。
使用深度缓存便可以很巧妙地解决这个问题。Z-Buffer 是 Shadowing Mapping 算法的技术基础。Shadowing Mapping 借助 Z-Buffer 来做碰撞检测。具体则是通过将深度值存储到纹理中，从而可以对光源的透视图所见的最近深度值进行采样，最终深度值就会显示从光源的透视图下见到的第一个片元了， 从而决定其他片元是否在阴影中。

### 深度贴图

> 深度映射由两个步骤组成：首先，我们渲染深度贴图，然后我们像往常一样渲染场景，使用生成的深度贴图来计算片元是否在阴影之中。

- 创建一个帧缓冲对象。
```c++
GLuint depthMapFBO;
glGenFramebuffers(1, &depthMapFBO);
```

- 创建2D纹理给帧缓冲的深度缓冲使用。生成深度贴图只关心深度值，因此把纹理格式指定为 GL_DEPTH_COMPONENT。
```c++
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

GLuint depthMap;
glGenTextures(1, &depthMap);
glBindTexture(GL_TEXTURE_2D, depthMap);
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
             SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
```

- 生成深度贴图的基本流程。
```c++
// 1. 首选渲染深度贴图
glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    ConfigureShaderAndMatrices();
    RenderScene();
glBindFramebuffer(GL_FRAMEBUFFER, 0);
// 2. 像往常一样渲染场景，但这次使用深度贴图
glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
ConfigureShaderAndMatrices();
glBindTexture(GL_TEXTURE_2D, depthMap);
RenderScene();
```

### 光源空间的变换
之所以需要光源空间变换，是因为我们需要对摄像机视角看到的片元做出是否在阴影中的判断，而这样的判断需要在光源的视角下进行。

- 这需要结合光的视图矩阵和投影矩阵。
```c++
glm::mat4 lightSpaceMatrix = lightProjection * lightView;
```

- 渲染到深度贴图。
```c++
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
}
```

### 渲染阴影
最复杂的一步，不过好在光照模型在上个作业中有做过。main函数的逻辑基本比较容易理解。不过这里加了个阴影的计算，在阴影计算的过程中使用透视除法。

```c++
#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    [...]
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1.0);
    // Ambient
    vec3 ambient = 0.15 * color;
    // Diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // 计算阴影
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    FragColor = vec4(lighting, 1.0f);
}
```

- 阴影计算的过程。前面介绍过其主要思想，就是对比目标点与最近点的深度，如果目标点深度比最近点更深，那么他就应该处在阴影中。
```c++
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // Check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}
```

### 优化
解决锯齿问题。因为阴影贴图受限于解析度，在距离光源比较远的情况下，多个片元可能从深度贴图的同一个值中去采样。这样只需要加一个简单的偏移量即可。

- 对表面深度做一个微小偏移即可，通常0.005的偏移量就能解决大部分问题。但有些表面坡度较大，不平整，这样保险起见还是对表面朝向光线的角度更改偏移量。
```c++
float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
```

## 主要的function/algorithm解释
- **glm::mat4(1.0f)。** 生成一个4*4的单位矩阵，用于配合后面的变换函数生成变换矩阵。
- **glm::translate(mat4, glm::vec3)。** 传入一个三维向量表示位移量，glm根据这个位移量，并在mat4的基础上，加入位移变换，生成新的矩阵并返回。
- **glm::perspective(mat4, width/height, near, far)。** 第一个参数是view矩阵，第二个参数是窗口的宽高比，前两个参数基本固定。后两个参数定义最近、最远能看到的平面的垂直距离。
- **glm::lookAt(position, target, up)。** 该函数可以产生一个效果是始终看着某一点的view矩阵。三个参数都是一个glm::vec3类型的向量，分别指定摄像机的位置、观察目标和上向量（用于定位摄像机的角度）。
- **glGetUniformLocation(shaderProgram, uniformName)。** 这个函数返回着色器程序中uniform变量的地址，如果没有找到返回-1。第一个参数是链接后的着色器程序的id（不是顶点着色器的id，一开始传错参数坑了很久）。第二个参数是要找的uniform变量的名称。
- **glUniformMatrix4fv(uniformLocation, count, transpose, address)。** 该函数的作用是对着色器程序中uniform变量的赋值。第一个参数是上个函数返回的uniform变量在着色器程序中的地址，第二个参数表示要改变的uniform变量的个数，传1表示要修改的uniform变量不是数组。第三个参数是布尔类型，指定传进去的变换矩阵是否要做转置。第四个参数则是要传进去的变换矩阵的地址。
- **glUniform3fv(uniformLocation, count, address)。** 类似上面的函数，不过是传一个三维的浮点数向量，不用指定transpose。
- **glUniform1fv(uniformLocation, count, address)。** 类似上面的函数，传一个一维的浮点数向量。
- **glUniform1iv(uniformLocation, count, address)。** 类似上面的函数，传一个一维的整数向量。
- 其他glUniform函数以此类推。