#ifndef _ELI_MATH_
#define _ELI_MATH_

#define _USE_MATRIX_WITH_EIGEN_

#if defined(_USE_MATRIX_WITH_EIGEN_)

#elif defined(_DALK_USE_DIRECT3D_)
#define _USE_MATRIX_WITH_D3DX_
#elif defined(_DALK_USE_OPENGL_)
#define _USE_MATRIX_WITH_GLM_
#endif

//Header
#if defined(_USE_MATRIX_WITH_EIGEN_)
#include <Eigen/Eigen/Core>
#include <Eigen/Eigen/Geometry>
#elif defined(_USE_MATRIX_WITH_BOOST_)
#include <math.h>
#include <boost/numeric/ublas/matrix.hpp> 
#elif defined (_USE_MATRIX_WITH_GLM_)
#import <glm/glm.hpp>	
#import <glm/gtc/matrix_transform.hpp>
#import <glm/gtc/type_ptr.hpp>
#elif defined (_USE_MATRIX_WITH_D3DX_)
#include <d3dx9.h>
#endif

#if defined(_DALK_USE_OPENGL_)
#include <Siglune/SigD3DBridge.h>
#endif

//π
#if defined(_USE_MATRIX_WITH_EIGEN_)
#define _PI ((float) 3.141592654f)
#elif defined(_USE_MATRIX_WITH_BOOST_)
#define _PI  M_PI
#elif defined (_USE_MATRIX_WITH_GLM_)
#define _PI ((FLOAT) 3.141592654f)
#elif defined (_USE_MATRIX_WITH_D3DX_)
#define _PI  D3DX_PI
#endif

//Matrix
#if defined(_USE_MATRIX_WITH_EIGEN_)

#if defined(_DALK_USE_DIRECT3D_)
typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor>  _MATRIX;
#else
typedef Eigen::Matrix4f  _MATRIX;
#endif

typedef Eigen::Vector3f _VECTOR;
typedef Eigen::Vector2f _VECTOR2;
typedef Eigen::Vector3f _VECTOR3;
typedef Eigen::Vector4f _VECTOR4;

#define _MATRIXIDENTITY(mat)  mat=_MATRIX::Identity()

inline void _MATRIXSCALE(_MATRIX& mat, float x, float y, float z) {
	_MATRIXIDENTITY(mat);
	mat(0, 0) = x;
	mat(1, 1) = y;
	mat(2, 2) = y;
//	Eigen::DiagonalMatrix<float, 3> scaling = Eigen::Scaling(x, y, z);
//	Eigen::Affine3f _affine(scaling);
//	mat *= _affine.matrix;
}

inline void _MATRIXTRANSLATE(_MATRIX& mat, float x, float y, float z) {
	_MATRIXIDENTITY(mat);
	mat(0, 0) = x;
	mat(1, 1) = y;
	mat(2, 2) = y;
//	Eigen::Translation<float, 3> translation = Eigen::Translation<float, 3>(x, y, z);
//	Eigen::Affine3f _affine(translation);
//	mat *= _affine.matrix;
}

inline void  _MATRIXROTATEX(_MATRIX& mat, float rad) {
	_MATRIXIDENTITY(mat);
	mat(1, 1) = cos(rad);
	mat(2, 2) = cos(rad);
	mat(1, 2) = sin(rad);
	mat(2, 1) = -sin(rad);

//	Eigen::Quaternionf rotate(Eigen::AngleAxisf(rad, Eigen::Vector3f::UnitX()));
//	Eigen::Affine3f _affine(rotate);
//	mat *= _affine.matrix;
}

inline void _MATRIXROTATEY(_MATRIX& mat, float rad) {
	_MATRIXIDENTITY(mat);
	mat(0, 0) = cos(rad);
	mat(2, 2) = cos(rad);
	mat(0, 2) = -sin(rad);
	mat(2, 0) = sin(rad);
//	Eigen::Quaternionf rotate(Eigen::AngleAxisf(rad, Eigen::Vector3f::UnitY()));
//	Eigen::Affine3f _affine(rotate);
//	mat *= _affine.matrix;
}
inline void  _MATRIXROTATEZ(_MATRIX& mat, float rad) {
	_MATRIXIDENTITY(mat);
	mat(0, 0) = cos(rad);
	mat(1, 1) = cos(rad);
	mat(0, 1) = sin(rad);
	mat(1, 0) = -sin(rad);
	
//	Eigen::Quaternionf rotate(Eigen::AngleAxisf(rad, Eigen::Vector3f::UnitZ()));
//	Eigen::Affine3f _affine(rotate);
//	mat *= _affine.matrix;
}

inline void _MATRIXROTATE(_MATRIX& mat, float x, float y, float z)	{
	FLOAT sroll, croll, spitch, cpitch, syaw, cyaw;
//	TRACE("out %p, yaw %f, pitch %f, roll %f\n", out, yaw, pitch, roll);
	sroll = sinf(z);
	croll = cosf(z);
	spitch = sinf(x);
	cpitch = cosf(x);
	syaw = sinf(y);
	cyaw = cosf(y);

	_MATRIXIDENTITY(mat);

	mat(0, 0) = sroll * spitch * syaw + croll * cyaw;
	mat(0, 1) = sroll * cpitch;
	mat(0, 2) = sroll * spitch * cyaw - croll * syaw;
	mat(0, 3) = 0.0f;
	mat(1, 0) = croll * spitch * syaw - sroll * cyaw;
	mat(1, 0) = croll * cpitch;
	mat(0, 1) = croll * spitch * cyaw + sroll * syaw;
	mat(1, 3) = 0.0f;
	mat(2, 0) = cpitch * syaw;
	mat(2, 1) = -spitch;
	mat(2, 2) = cpitch * cyaw;
	mat(2, 3) = 0.0f;
	mat(3, 0) = 0.0f;
	mat(3, 1) = 0.0f;
	mat(3, 2) = 0.0f;
	mat(3, 3) = 1.0f;
	/*
	Eigen::AngleAxisf rollAngle(x, Eigen::Vector3f::UnitZ());
	Eigen::AngleAxisf yawAngle(y, Eigen::Vector3f::UnitY());
	Eigen::AngleAxisf pitchAngle(z, Eigen::Vector3f::UnitX());

	Eigen::Quaternionf qf = rollAngle * yawAngle * pitchAngle;
	Eigen::Affine3f _affine(qf);
	mat *= _affine.matrix;
	*/
}

#define _MATRIXTRANSPOSE(mat) mat=mat.transpose()
#define _MATRIXARRAYPTR(mat) (mat.data())

inline void _MATRIXPERSPECTIVE(_MATRIX& mat, float fovy, float aspect, float zNear, float zFar )
{
//	assert(aspect > 0.0f);
//	assert(zFar > zNear);

	mat = _MATRIX::Zero();

#if defined(_DALK_WINDOWS_)
	mat(0, 0) = 1.0f / (aspect * tanf(fovy / 2.0f));
	mat(1, 1) = 1.0f / tanf(fovy / 2.0f);
	mat(2, 2) = zFar / (zFar - zNear);
	mat(2, 3) = 1.0;
	mat(3, 2) = (zFar * zNear) / (zNear - zFar);
	mat(3, 3) = 0.0;
#elif defined(_DALK_IOS_)
//	float radf = Math::degToRad(fovy);
	float tanHalfFovy = tan(fovy / 2.0);

	mat(0, 0) = 1.0 / (aspect * tanHalfFovy);
	mat(1, 1) = 1.0 / (tanHalfFovy);
	mat(2, 2) = -(zFar + zNear) / (zFar - zNear);
	mat(3, 2) = -1.0;
	mat(2, 3) = -(2.0 * zFar * zNear) / (zFar - zNear);
//	return res;
#endif
}

inline void _MATRIXORTHO(_MATRIX& mat, float left, float right, float bottom, float top, float zNear, float zFar)
{
	//TRACE("pout %p, w %f, h %f, zn %f, zf %f\n", pout, w, h, zn, zf);
	_MATRIXIDENTITY(mat);
	mat(0, 0) = 2.0f / right - left;
	mat(1, 1) = 2.0f / bottom - top;
	mat(2, 2) = 1.0f / (zFar - zNear);
	mat(3, 2) = zNear / (zNear - zFar);
}

inline void _MATRIXFRUSTUM(_MATRIX& mat, float left, float right, float bottom, float top, float zNear, float zFar)
{

}

inline void _MATRIXLOOKAT(_MATRIX& mat, _VECTOR& Eye, _VECTOR& At, _VECTOR& Up)
{
	_VECTOR f = (At - Eye).normalized();
	_VECTOR u = Up.normalized();
	_VECTOR s = f.cross(u).normalized();
	u = s.cross(f);

	_MATRIX res;
	res << s.x(),  s.y(),  s.z(), -s.dot(Eye),
		   u.x(),  u.y(),  u.z(), -u.dot(Eye),
		  -f.x(), -f.y(), -f.z(),  f.dot(Eye),
	       0, 0, 0, 1;
}

#if defined(_DALK_USE_DIRECT3D_)
#define _MATRIXARRAYD3DPTR(mat) ((D3DMATRIX*)(mat.data()))
#define _MATRIXARRAYD3DXPTR(mat) ((D3DXMATRIX*)(mat.data()))
#define _VECTOR3D3DPTR(vec) ((D3DVECTOR3*)(vec.data()))
#define _VECTOR3D3DXPTR(vec) ((D3DXVECTOR3*)(vec.data()))
#endif

#elif defined(_USE_MATRIX_WITH_BOOST_)
 typedef matrix<float> _MATRIX;
 typedef vector<float> _VECTOR;
#elif defined (_USE_MATRIX_WITH_GLM_)
 typedef glm::mat4 _MATRIX;
 typedef glm::vec3 _VECTOR;
 typedef glm::vec2 _VECTOR2;
 typedef glm::vec3 _VECTOR3;
 typedef glm::vec4 _VECTOR4;
#define _MATRIXIDENTITY(mat)			mat = glm::mat4(1.0)
#define _MATRIXSCALE(mat, x, y, z)		mat = glm::scale(mat, glm::vec3(x,y,z))
#define _MATRIXTRANSLATE(mat, x, y, z)	mat = glm::translate(mat, glm::vec3(x,y,z))
#define _MATRIXROTATE(mat, x, y, z)		mat = glm::rotate(mat, 0,0f, glm::vec3(x,y,z))
#define _MATRIXROTATEX(mat, rad)		mat = glm::rotate(mat, rad, glm::vec3(1.0f, 0.0f, 0.0f))
#define _MATRIXROTATEY(mat, rad)		mat = glm::rotate(mat, rad, glm::vec3(0.0f, 1.0f, 0.0f))
#define _MATRIXROTATEZ(mat, rad)		mat = glm::rotate(mat, rad, glm::vec3(0.0f, 0.0f, 1.0f))
#define _MATRIXTRANSPOSE(mat)			(glm::transpose(proj))
#define _MATRIXARRAYPTR(mat)			(glm::value_ptr(mat))
#define _MATRIXPERSPECTIVE( mat, fovy, aspect, znear, zfar )		mat = glm::perspective(fovy, aspect, znear, zfar)
#define _MATRIXORTHO( mat, left, right, bottom, top, zNear, zfar)	mat = glm::ortho(left, right, bottom, top, zNear, zFar)
#define _MATRIXFRUSTUM( mat, left, right, bottom, top, zNear, zfar) mat = glm::frustum(left, right, bottom, top, zNear, zFar)
#define _MATRIXLOOKAT( mat, Eye, At, Up)							mat = glm::lookAt(Eye, At, Up)

#elif defined (_USE_MATRIX_WITH_D3DX_)
 typedef D3DXMATRIX _MATRIX;
 typedef D3DXVECTOR3 _VECTOR;
 typedef D3DXVECTOR2 _VECTOR2;
 typedef D3DXVECTOR3 _VECTOR3;
 typedef D3DXVECTOR4 _VECTOR4;
#define _MATRIXIDENTITY(mat)			D3DXMatrixIdentity(&mat)
#define _MATRIXSCALE(mat, x, y, z)		D3DXMatrixScaling(&mat, x,y,z)
#define _MATRIXTRANSLATE(mat, x, y, z)	D3DXMatrixTranslation(&mat, x,y,z)
#define _MATRIXROTATE(mat, x, y, z)		D3DXMatrixRotationYawPitchRoll (&mat, x,y,z)
#define _MATRIXROTATEX(mat, rad)	D3DXMatrixRotationX (&mat, rad)
#define _MATRIXROTATEY(mat, rad)	D3DXMatrixRotationY (&mat, rad)
#define _MATRIXROTATEZ(mat, rad)	D3DXMatrixRotationZ (&mat, rad)
#define _MATRIXARRAYPTR(mat) (&mat)
#define _MATRIXPERSPECTIVE( mat, fovy, aspect, znear, zfar )		D3DXMatrixPerspectiveFovLH( &mat, fovy, aspect, znear, zfar);
#define _MATRIXORTHO( mat, left, right, bottom, top, zNear, zfar)	D3DXMatrixOrthoLH( mat, right - left + 1, bottom - top + 1, znear, zfar)
#define _MATRIXFRUSTUM( mat, left, right, bottom, top, zNear, zfar) 
#define _MATRIXLOOKAT( mat, Eye, At, Up)							D3DXMatrixLookAtLH( &mat, &Eye, &At, &Up)

#endif

#endif
