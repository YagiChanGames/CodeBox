struct material{
    std::string material_name;
    
    std::string diffuse_texture_name;
    std::string normal_texture_name;
    std::string specular_texture_name;
    std::string falloff_texture_name;
    std::stringreflection_map_texture_name;
    XMFLOAT3 diffuse;
    bool outline=false;
    bool alpha=false;
    bool double_sided=false;
};

material LoadMaterial(FbxSurfaceMaterial* mat)
{
    material model_material;
    model_material.material_name=mat->GetName();
    
    DXUTTRACE(L">>material: %S¥n",model_material.material_name.c_str());
    
    auto implementation=GetImplementation(mat,FBXSDK?IMPLEMENTATION_CGFX);
    if(implementation){
        outo root_table=implementation->GetRootTable();
        auto entry_count=root_table->GetEntryCount();
        
        for(std::size_t i=0;i<entry_count;++i){
            auto entry=root_table->GetEntry(i);
            
            auto fbx_property=mat->FindPropertyHierarchical(entry.GetSource());
            if(!fbx_property.IsValid()){
                fbx_property=mat->RootProperty.FindPropertyHierarchical(entry.GetSource());
            }
            std::string stc=entry.GetSource();
            if(src=="Maya|DiffuseTexture"){
                model_material.diffuse_texture_name=fbx_property.GetSrcObject<FbxFileTexture>(0)->GetFileName();
            }
            else if(src=="Maya|NormalTexture"){
                model_material.normal_texture_name=fbx_property.GetSrcObject<FbxFileTexture>(0)->GetFileName();
            }
            else if(src=="Maya|SpecularTexture"){
                model_material.specular_texture_name=fbx_property.GetSrcObject<FbxFileTexture>(0)->GetFileName();
            }
            else if(src=="Maya|FalloffTexture"){
                model_material.falloff_texture_name=fbx_property.GetSrcObject<FbxFileTexture>(0)->GetFileName();
            }
            else if(src=="Maya|ReflectionMapTexture"){
                model_material.reflection_map_texture_name=fbx_property.GetSrcObject<FbxFileTexture>(0)->GetFileName();
            }
            else if(src=="Maya|g_LitColor"){
                FbxDouble3 lDouble3=fbx_property.Get<FbxDouble3>();
                model_material.diffuse.x=static_cast<float>(lDouble3[0]);
                model_material.diffuse.y=static_cast<float>(lDouble3[1]);
                model_material.diffuse.z=static_cast<float>(lDouble3[2]);
            }
            else if(src=="Maya|g_bOutlineEnabke"){
                model_material.outline=fbx_property.Get<FbxBool>();
            }
            else if(src=="Maya|g_bAlphaEnabke"){
                model_material.alpha=fbx_property.Get<FbxBool>();
            }
            else if(src=="Maya|g_bDoubleSided"){
                model_material.double_sided=fbx_property.Get<FbxBool>();
            }
            else {
                DXUTTRACE(L">>>> unknown prop: %S¥n",src.c_str());
            }
        }
        
        DXUTTRACE(L"diffuseTexture: %S¥n",model_material.diffuse_texture_name.c_str());
        DXUTTRACE(L"normalTexture: %S¥n",model_material.normal_texture_name.c_str());
        
    }
}