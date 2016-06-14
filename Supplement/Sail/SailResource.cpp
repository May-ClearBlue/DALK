#include "Multistdafx.h"

#include "SailResource.h"
/*
BOOL template <class P>CResourceManager::AddResource(string Name,P Resource,BOOL bOverWrite = FALSE){
	std::hash_map<string,P>::iterator iResource;
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

P* CResourceManager::FindResource(string Name){
	std::hash_map<string,P>::iterator iResource;

	iResource = m_oResource.find(Name);
	return (iResource != m_oResource.end() ) ? iResource.second : NULL;
}
*/