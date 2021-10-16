#pragma once

#define		CheckPointer(p, ret) if(0==p){return ret;}
#define		CheckRange(a, min, max) if(a <min || a > max){return E_INVALIDARG;}
#define		SafeDeleteArray(p) if(p){delete[] p;}