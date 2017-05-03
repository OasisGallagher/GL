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
column major��row major��ʾ���Ǿ������ڴ��еĸ�ʽ. OpenGL�й���column major, DirectX����row major.
���ֱ�ʾ��ʽ���ڴ沼������ͬ��, ���Ǳ��ľ�������ѧ���ǲ�ͬ��.
OpenGL��, ������ϰ�ߵ���������, ����ڴ�������������ĳ�ʱ, ����post-multiply.
����, 
����column major��ʾ, �ڴ���Ϊ{ 1, 3, 2, 4 }�ľ���, ʵ��Ϊ:
1 2 
3 4, ��������(x, y)���ʱ(1 * x + 2 * y, 3 * x + 4 * y).
����row major��ʾ, �ڴ���ͬ��Ϊ{ 1, 3, 2, 4 }�ľ���, ʵ��Ϊ:
1 3
2 4, ��column major�����ת��. �����������ʱ, ������ʾΪ������, ��pre-multiply.
����(x, y)��þ������ʱ, �õ�, (x * 1 + y * 2, x * 3 + y * 4).
��column major�õ��Ľ������ͬ��.
OpenGL��ʹ��column����row majorû��ǿ�ƹ涨, ֻ�ǹ���column major. 

How to find camera position and rotation from a 4x4 matrix?
View matrix��������������ϵ������ת�����������.
�����:
W = View * C
W����������ϵԭ��(0, 0, 0, 1), C���������λ��.
����: C = inverse(View) * W = invise(View)[3].
�����������:
X = (View[0][0], View[1][0], View[2][0])
Y = (View[0][1], View[1][1], View[2][1])
Z = (View[0][2], View[1][2], View[2][2])