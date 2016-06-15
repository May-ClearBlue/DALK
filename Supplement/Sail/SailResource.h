#ifndef _SAIL_RESOURCE_
#define _SAIL_RESOURCE_

#include "MultiStdAfx.h"

#if 0
class CResource {
protected:
	string_t	m_ResourceName;
public:
		string_t&	GetName(){ return m_ResourceName; }
virtual	BOOL	Release() = 0;
};

template <class P>
class CResourceManager{
protected:
	hash_map   <string_t,P*>	m_oResource;
public:
	BOOL	AddResource(string_t Name,P* Resource,BOOL bOverWrite = FALSE){
				hash_map<string_t,P*>::iterator iResource;
				iResource = m_oResource.find(Name);

				if( iResource == m_oResource.end() ){
					m_oResource[Name] = Resource;
					return TRUE;
				}

				else if(bOverWrite){
					m_oResource.erase(iResource);
					m_oResource[Name] = Resource;
					return TRUE;
				}

				return FALSE;
			}

	P*		FindResource(string_t Name){
				std::hash_map<string_t,P*>::iterator iResource;

				iResource = m_oResource.find(Name);

				return (iResource != m_oResource.end() ) ? (*iResource).second : NULL;
			}

	P*		FindResource(unsigned int Number){
				if( Number >= m_oResource.size() )
					return NULL;

				std::hash_map<string_t,P*>::iterator iResource = m_oResource.begin();
				for(unsigned int a = 0;a < Number; a++)
				//if(Number > 0)
					iResource ++;//= Number;
				return (*iResource).second;
			}
	P*		operator[](char* Name){ return m_oResource[Name]; }
};

#endif

#endif