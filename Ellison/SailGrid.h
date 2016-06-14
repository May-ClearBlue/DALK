#ifndef _SAIL_GRID_
#define _SAIL_GRID_

#include "SailSprite.h"

class IGrid{
protected:
	int		m_GridWidth,m_GridHeight;
	int		m_GridX,m_GridY;
public:
virtual	void	SetGrid(int width,int height) = 0;
virtual	void	Draw(int GridNum,int x,int y) = 0;
virtual	void	Load(char* Name) = 0;
};

class CGrid:public CSprite2D{
protected:
	int		m_GridWidth,m_GridHeight;
	int		m_GridX,m_GridY;
public:
	void	SetGrid(int width,int height);
	void	DrawGrid(int GridNum,int x,int y);
};

#endif