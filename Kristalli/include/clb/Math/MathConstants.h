/** @file MathConstants.h
	@author Jukka Jyl�nki

	This work is copyrighted material and may NOT be used for any kind of commercial or 
	personal advantage and may NOT be copied or redistributed without prior consent
	of the author(s). 

	@brief Common mathematical constants.

	See
	http://www.worldwideschool.org/library/books/sci/math/MiscellaneousMathematicalConstants/toc.html

*/

#ifndef MathConstants_h
#define MathConstants_h

#include "clb/Math/MathSettings.h"

namespace clb
{
/// The mathematical core classes and functions for clbLib.
namespace math
{
	/// \f$\frac{1}{\sqrt{2\pi}}\f$
	const real recipSqrt2Pi = (real)0.3989422804014326779399460599343818684758586311649346576659258296706579258993018385012523339073069364;
	/// \f$\cos{1}\f$
	const real cos1 =         (real)0.5403023058681397174009366074429766037323104206179222276700972553811003947744717645179518560871830893;
	/// \f$\ln{2}\f$
	const real ln2 =          (real)0.6931471805599453094172321214581765680755001343602552541206800094933936219696947156058633269964186875;
	/// \f$\sqrt[3]{3}\f$
	const real cbrt3 =        (real)1.4422495703074083823216383107801095883918692534993505775464161945416875968299973398547554797056452567;
	/// \f$\frac{1}{\ln{2}}\f$
	const real recipLn2 =     (real)1.4426950408889634073599246810018921374266459541529859341354494069311092191811850798855266228935063444;
	/// Golden ratio, or \f$\frac{1+\sqrt{5}}{2}\f$
	const real phi =          (real)1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911375;
	/// \f$\ln{10}\f$
	const real ln10 =         (real)2.3025850929940456840179914546843642076011014886287729760333279009675726096773524802359972050895982983;
	/// \f$e\f$
	const real e  =           (real)2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274;
	/// \f$\pi\f$
	const real pi =           (real)3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
	/// \f$e^2\f$
	const real e2 =           (real)7.3890560989306502272304274605750078131803155705518473240871278225225737960790577633843124850791217948;
}
}

#endif
