// CubicSplineInterpolation.h: interface for the CCubicSplineInterpolation class.
//
/*/////////////////////////////////////////////////////////////////////
*
*class CCubicSplineInterpolation  三次样插值计算类
*作者 *QQ：29720770
*把网上流传的算法，改造成一个方便使用的C++类
*本类对点的X Y值采用 double 数组，使用时可以从此类继承一个类，封装成你自己的类型
*
*使用接口 
* CCubicSplineInterpolation(double *ctrlPtX,double *ctrlPtY,int nCtrlPtCount);  输入控制点，构造函数
* bool GetInterpolationPts(int outPtCount,double* outPtX,double* outPtY);       获得插值后的点数组
* bool GetYByX(const double &dbInX, double &dbOutY);                            获得单个X值所对应的Y值，即反算
*
*//////////////////////////////////////////////////////////////////////
#if !defined(AFX_CUBICSPLINEINTERPOLATION_H__58C0304E_719D_4738_B86C_26BFA529B203__INCLUDED_)
#define AFX_CUBICSPLINEINTERPOLATION_H__58C0304E_719D_4738_B86C_26BFA529B203__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CCubicSplineInterpolation  
{
public:

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//
	//ctrlPtX 控制点X数组
	//ctrlPtY 控制点Y数组
	//nCtrlPtCount 控制点数目，控制点要大于等于3.
	//
	//////////////////////////////////////////////////////////////////////
	CCubicSplineInterpolation(double *ctrlPtX,double *ctrlPtY,int nCtrlPtCount);
	virtual ~CCubicSplineInterpolation();
public:

	//////////////////////////////////////////////////////////////////////////
	//outPtCount 想要输出的插值点数目,输出的点数组要大于1
	//outPtX     已经分配好内存的X值的数组。
	//outPtY     已经分配好内存的Y值的数组。
	//
	//调用此函数，获得插值点数组
	//
	//计算成功返回true，计算失败返回false
	//////////////////////////////////////////////////////////////////////////
	bool GetInterpolationPts(int outPtCount,double* outPtX,double* outPtY);
	//////////////////////////////////////////////////////////////////////////
	//根据X值 计算Y值
	//dbInX   x自变量值，输入
	//dbOutY  计算得到的Y值 输出
	//////////////////////////////////////////////////////////////////////////
	bool GetYByX(const double &dbInX, double &dbOutY);
protected:
	void ReleaseMem();
	void InitParam();
	bool InterPolation();
	bool Spline();
protected:
	bool m_bCreate; //类是否创建成功，即控制点是否有效
	int N;   //输入控制点数量
	int M;   //输出的插入点数量
	typedef double* pDouble;
	pDouble X,Y; //输入的控制点数组
	pDouble Z,F; //输出的控制点数组
	pDouble H,A,B,C,D; //间距，缓存运算中间结果。
};
#endif // !defined(AFX_CUBICSPLINEINTERPOLATION_H__58C0304E_719D_4738_B86C_26BFA529B203__INCLUDED_)
