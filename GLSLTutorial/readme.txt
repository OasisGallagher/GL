History:
http://www.cnblogs.com/vertexshader/articles/2917540.html

Tutorial:
http://www.lighthouse3d.com/tutorials/glsl-tutorial/
	
How does the fragment shader know what variable to use for the color of a pixel?
http://stackoverflow.com/questions/9222217/how-does-the-fragment-shader-know-what-variable-to-use-for-the-color-of-a-pixel

How do you get the modelview and projection matrices in OpenGL?
http://stackoverflow.com/questions/4202456/how-do-you-get-the-modelview-and-projection-matrices-in-opengl

Column-major matrix vs row-major matrix.
http://stackoverflow.com/questions/17717600/confusion-between-c-and-opengl-matrix-order-row-major-vs-column-major
column major和row major表示的是矩阵在内存中的格式. OpenGL中惯用column major, DirectX惯用row major.
两种表示方式的内存布局是相同的, 但是表达的矩阵在数学上是不同的.
OpenGL中, 把向量习惯当做列向量, 因此在处理矩阵与向量的乘时, 采用post-multiply.
例如, 
采用column major表示, 内存中为{ 1, 3, 2, 4 }的矩阵, 实际为:
1 2 
3 4, 在与向量(x, y)相乘时(1 * x + 2 * y, 3 * x + 4 * y).
采用row major表示, 内存中同样为{ 1, 3, 2, 4 }的矩阵, 实际为:
1 3
2 4, 是column major矩阵的转置. 在与向量相乘时, 向量表示为行向量, 且pre-multiply.
向量(x, y)与该矩阵相乘时, 得到, (x * 1 + y * 2, x * 3 + y * 4).
与column major得到的结果是相同的.
OpenGL对使用column还是row major没有强制规定, 只是惯用column major. 

How to find camera position and rotation from a 4x4 matrix?
View matrix用来把世界坐标系的坐标转换到相机坐标.
因此有:
W = View * C
W是世界坐标系原点(0, 0, 0, 1), C是照相机的位置.
所以: C = inverse(View) * W = invise(View)[3].
对于相机朝向:
X = (View[0][0], View[1][0], View[2][0])
Y = (View[0][1], View[1][1], View[2][1])
Z = (View[0][2], View[1][2], View[2][2])