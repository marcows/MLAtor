#ifndef _MLA_COMPAT_H
#define _MLA_COMPAT_H

// unknown qualifier for program memory
#define __prog__ const

// for compiling GOLFontDefault, this switch is not used anywhere else
#define __XC32__

// No Operation, defined in Compiler.h
#define Nop()

// ATTENTION: including Graphics/GOL.h directly or indirectly here before all
// others makes the GFX_SCHEMEDEFAULT system non-working if not defined because
// it is defined in GOL.h then and thus GOLSchemeDefault does not get compiled.
//#include <Graphics/Graphics.h>
//void TouchGetMsg(GOL_MSG *pMsg);

#endif
