#include "shadermanager.h"

#include "Managers/assetmanager.h"

ShaderManager::ShaderManager()
{
    qDebug() << "\n\nINITIALIZING SHADER MANAGER";
    AddShader(PHONG_SHADER); //0
    qDebug() << "Phong shader program id: " << shaders_[0]->program_;
    AddShader(TEXTURE_SHADER); //1
    qDebug() << "Texture shader program id: " << shaders_[1]->program_;
    AddShader(PLAIN_SHADER); //2
    qDebug() << "plain shader program id: " << shaders_[2]->program_;
    AddShader(MONO_COLOR_SHADER); //3
    qDebug() << "Outline shader program id: " << shaders_[3]->program_;

}

void ShaderManager::SetActiveCamera(std::shared_ptr<Camera> camera)
{
    for(auto shader : shaders_)
    {
        shader->currentCamera_ = camera;
    }
}

void ShaderManager::AddShader(ShaderType shaderType)
{
    switch (shaderType) {
    case PLAIN_SHADER:
        shaders_.push_back(std::make_shared<Shader>("PlainShader", shaderType));
        LoadShadersFromFiles("plainshader");
        shaders_.back()->mMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "mMatrix" );
        shaders_.back()->vMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "vMatrix" );
        shaders_.back()->pMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "pMatrix" );

        break;
    case TEXTURE_SHADER:
        shaders_.push_back(std::make_shared<Shader>("TextureShader", shaderType));
        LoadShadersFromFiles("textureshader");
        shaders_.back()->mMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "mMatrix" );
        shaders_.back()->vMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "vMatrix" );
        shaders_.back()->pMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "pMatrix" );

        shaders_.back()->objectColorUniform_ = glGetUniformLocation( shaders_.back()->program_, "objectColor" );
        shaders_.back()->textureUniform_ = glGetUniformLocation(shaders_.back()->program_, "textureSampler");

        break;
    case PHONG_SHADER:
    {
        shaders_.push_back(std::make_shared<Shader>("PhongShader", shaderType));
        LoadShadersFromFiles("lightshader");
        shaders_.back()->mMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "mMatrix" );
        shaders_.back()->vMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "vMatrix" );
        shaders_.back()->pMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "pMatrix" );

        shaders_.back()->numberOfPointLights_ = glGetUniformLocation( shaders_.back()->program_, "numberOfPointLights" );
        shaders_.back()->numberOfSpotLights_ = glGetUniformLocation( shaders_.back()->program_, "numberOfSpotLights" );
        shaders_.back()->numberOfDirectionalLights_ = glGetUniformLocation( shaders_.back()->program_, "numberOfDirectionalLights" );

        for(int i = 0; i < gsl::MAX_NUMBER_OF_LIGHTS; i++)
        {
            std::string light{""};
            std::string index = "[" + std::to_string(i) + "].";
            std::string variabel{""};

            light = "pointLight";
            variabel = "position";
            shaders_.back()->pointLight_[i].position = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "ambient";
            shaders_.back()->pointLight_[i].ambient = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "diffuse";
            shaders_.back()->pointLight_[i].diffuse = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "specular";
            shaders_.back()->pointLight_[i].specular = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "constant";
            shaders_.back()->pointLight_[i].constant = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "linear";
            shaders_.back()->pointLight_[i].linear = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "quadratic";
            shaders_.back()->pointLight_[i].quadratic = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );

            light = "spotLight";
            variabel = "position";
            shaders_.back()->spotLight_[i].position = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "direction";
            shaders_.back()->spotLight_[i].direction = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "cutOff";
            shaders_.back()->spotLight_[i].cutOff = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "outerCutOff";
            shaders_.back()->spotLight_[i].outerCutOff = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "ambient";
            shaders_.back()->spotLight_[i].ambient = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "diffuse";
            shaders_.back()->spotLight_[i].diffuse = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "specular";
            shaders_.back()->spotLight_[i].specular = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );

            light = "dirLight";
            variabel = "direction";
            shaders_.back()->dirLight_[i].direction = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "ambient";
            shaders_.back()->dirLight_[i].ambient = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "diffuse";
            shaders_.back()->dirLight_[i].diffuse = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
            variabel = "specular";
            shaders_.back()->dirLight_[i].specular = glGetUniformLocation( shaders_.back()->program_, (light + index + variabel).c_str() );
        }
        // material info
        shaders_.back()->material_.color = glGetUniformLocation( shaders_.back()->program_, "material.color" );
        shaders_.back()->material_.diffuse = glGetUniformLocation( shaders_.back()->program_, "material.diffuse" );
        shaders_.back()->material_.specular = glGetUniformLocation( shaders_.back()->program_, "material.specular" );
        shaders_.back()->material_.shininess = glGetUniformLocation( shaders_.back()->program_, "material.shininess" );

        shaders_.back()->viewPosUniform_ = glGetUniformLocation( shaders_.back()->program_, "viewPos");
        break;
    }
    case MONO_COLOR_SHADER:
        shaders_.push_back(std::make_shared<Shader>("MonoColorShader", shaderType));
        LoadShadersFromFiles("MonoColorShader");
        shaders_.back()->mMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "mMatrix" );
        shaders_.back()->vMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "vMatrix" );
        shaders_.back()->pMatrixUniform_ = glGetUniformLocation( shaders_.back()->program_, "pMatrix" );

        shaders_.back()->objectColorUniform_ = glGetUniformLocation( shaders_.back()->program_, "objectColor" );
        break;
    }
}

void ShaderManager::LoadShadersFromFiles(std::string shaderName, const GLchar* geometryPath)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    std::string vertexName = shaderName + ".vert";
    std::string fragmentName = shaderName + ".frag";

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    std::string vertexWithPath = gsl::shaderFilePath.toStdString() + vertexName;
    std::string fragmentWithPath = gsl::shaderFilePath.toStdString() + fragmentName;

    // Open files and check for errors
    vShaderFile.open( vertexWithPath );
    if(!vShaderFile)
        qDebug() << "ERROR SHADER FILE " << QString::fromStdString(vertexWithPath) << " NOT SUCCESFULLY READ";
    fShaderFile.open( fragmentWithPath );
    if(!fShaderFile)
        qDebug() << "ERROR SHADER FILE " << QString::fromStdString(vertexWithPath) << " NOT SUCCESFULLY READ";
    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf( );
    fShaderStream << fShaderFile.rdbuf( );
    // close file handlers
    vShaderFile.close( );
    fShaderFile.close( );
    // Convert stream into string
    vertexCode = vShaderStream.str( );
    fragmentCode = fShaderStream.str( );

    const GLchar* vShaderCode = vertexCode.c_str( );
    const GLchar* fShaderCode = fragmentCode.c_str( );

    // Do the same thing with the geometry shader if it exists:
    const GLchar* gShaderCode = nullptr;
    std::string geometryCode;

    if (geometryPath)
    {
        std::string geometryCode;
        std::ifstream gShaderFile;
        // Open files and check for errors
        gShaderFile.open( geometryPath );
        if(!gShaderFile)
            qDebug() << "ERROR SHADER FILE " << QString::fromStdString(geometryPath) << " NOT SUCCESFULLY READ";

        std::stringstream gShaderStream;
        // Read file's buffer contents into streams
        gShaderStream << gShaderFile.rdbuf( );
        // close file handlers
        gShaderFile.close( );
        // Convert stream into string
        geometryCode = gShaderStream.str( );

        gShaderCode = geometryCode.c_str( );
    }

    // 2. Compile shaders
    GLuint vertex{0}, fragment{0}, geometry{0};
    GLint success{0};
    GLchar infoLog[512]{};

    // Vertex Shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, nullptr );
    glCompileShader( vertex );
    // Print compile errors if any
    glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertex, 512, nullptr, infoLog );
        qDebug() << "ERROR SHADER VERTEX " << QString::fromStdString(shaderName) << " COMPILATION_FAILED\n" << QString::fromStdString(infoLog);
    }
    // Fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, nullptr );
    glCompileShader( fragment );
    // Print compile errors if any
    glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( fragment, 512, nullptr, infoLog );
        qDebug() << "ERROR SHADER FRAGMENT " << QString::fromStdString(shaderName) << " COMPILATION_FAILED\n" << QString::fromStdString(infoLog);
    }
    // Geometry Shader
    if (gShaderCode)
    {
        geometry = glCreateShader( GL_GEOMETRY_SHADER );
        glShaderSource( geometry, 1, &gShaderCode, nullptr );
        glCompileShader( geometry );
        // Print compile errors if any
        glGetShaderiv( geometry, GL_COMPILE_STATUS, &success );
        if ( !success )
        {
            glGetShaderInfoLog( geometry, 512, nullptr, infoLog );
            qDebug() << "ERROR SHADER GEOMETRY " << QString::fromStdString(geometryPath) << " COMPILATION_FAILED\n" << QString::fromStdString(infoLog);
        }
    }

    // Shader Program
    shaders_.back()->program_ = glCreateProgram( );
    glAttachShader( shaders_.back()->program_, vertex );
    glAttachShader( shaders_.back()->program_, fragment );
    if(geometryPath)
        glAttachShader( shaders_.back()->program_, geometry );
    glLinkProgram( shaders_.back()->program_ );
    // Print linking errors if any
    glGetProgramiv( shaders_.back()->program_, GL_LINK_STATUS, &success );
    if (!success)
    {
        glGetProgramInfoLog( shaders_.back()->program_, 512, nullptr, infoLog );
        qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << QString::fromStdString(infoLog);
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    if(geometryPath)
        glDeleteShader(geometry);

    qDebug() << "Shader read: " << QString::fromStdString(shaderName);
}

void ShaderManager::TransmitUniformDataToShader(size_t shaderID, gsl::Matrix4x4* modelMatrix, gsl::Vector3D color)
{
    glUseProgram(shaders_[shaderID]->program_);
    if (shaderID >= shaders_.size())
        return;

    glUniformMatrix4fv( shaders_[shaderID]->vMatrixUniform_, 1, GL_TRUE, shaders_[shaderID]->currentCamera_->viewMatrix_.constData());
    glUniformMatrix4fv( shaders_[shaderID]->pMatrixUniform_, 1, GL_TRUE, shaders_[shaderID]->currentCamera_->projectionMatrix_.constData());
    glUniformMatrix4fv( shaders_[shaderID]->mMatrixUniform_, 1, GL_TRUE, modelMatrix->constData());

    glUniform3f(shaders_[shaderID]->objectColorUniform_, color.x / gsl::MAX_COLORS, color.y / gsl::MAX_COLORS, color.z / gsl::MAX_COLORS);
}

void ShaderManager::TransmitUniformDataToShader(std::shared_ptr<Material> material, gsl::Matrix4x4* modelMatrix)
{   
    glUseProgram(shaders_[material->shaderID_]->program_);

    glUniformMatrix4fv( shaders_[material->shaderID_]->vMatrixUniform_, 1, GL_TRUE, shaders_[material->shaderID_]->currentCamera_->viewMatrix_.constData());
    glUniformMatrix4fv( shaders_[material->shaderID_]->pMatrixUniform_, 1, GL_TRUE, shaders_[material->shaderID_]->currentCamera_->projectionMatrix_.constData());
    glUniformMatrix4fv( shaders_[material->shaderID_]->mMatrixUniform_, 1, GL_TRUE, modelMatrix->constData());

    switch (shaders_[material->shaderID_]->type_) {
    case PLAIN_SHADER:
        break;
    case TEXTURE_SHADER:
        glUniform1i(shaders_[material->shaderID_]->textureUniform_, static_cast<int>(AssetManager::GetInstance()->GetTexture(material->textureID_)->glName_)); //TextureUnit = 0 as default);
        glUniform3f(shaders_[material->shaderID_]->objectColorUniform_, material->color_.x / gsl::MAX_COLORS, material->color_.y / gsl::MAX_COLORS, material->color_.z / gsl::MAX_COLORS);
        break;
    case PHONG_SHADER:
        glUniform3f(shaders_[material->shaderID_]->viewPosUniform_,
                shaders_[material->shaderID_]->currentCamera_->position_.x,
                shaders_[material->shaderID_]->currentCamera_->position_.y,
                shaders_[material->shaderID_]->currentCamera_->position_.z);
        glUniform3f(shaders_[material->shaderID_]->material_.color, material->color_.x / gsl::MAX_COLORS, material->color_.y / gsl::MAX_COLORS, material->color_.z / gsl::MAX_COLORS);
        glUniform1f(shaders_[material->shaderID_]->material_.shininess, material->shininess_);

        if (AssetManager::GetInstance()->GetTexture(material->textureID_))
            glUniform1i(shaders_[material->shaderID_]->material_.diffuse, static_cast<int>(AssetManager::GetInstance()->GetTexture(material->textureID_)->glName_));
        else
            glUniform1i(shaders_[material->shaderID_]->material_.diffuse, static_cast<int>(AssetManager::GetInstance()->GetTexture(0)->glName_));

        if (AssetManager::GetInstance()->GetTexture(material->specularMapID_))
            glUniform1i(shaders_[material->shaderID_]->material_.specular, static_cast<int>(AssetManager::GetInstance()->GetTexture(material->specularMapID_)->glName_));
        else
            glUniform1i(shaders_[material->shaderID_]->material_.specular, static_cast<int>(AssetManager::GetInstance()->GetTexture(0)->glName_));
        break;
    case MONO_COLOR_SHADER:
        glUniform3f(shaders_[material->shaderID_]->objectColorUniform_, material->color_.x, material->color_.y, material->color_.z);
        break;
    }
}

void ShaderManager::TransmitUniformLightDataToShader(size_t shaderID, int numberOfPointLights, int numberOfSpotLights, int numberOfDirectionalLights)
{
    glUseProgram(shaders_[shaderID]->program_);

    glUniform1i(shaders_[shaderID]->numberOfPointLights_, numberOfPointLights);
    glUniform1i(shaders_[shaderID]->numberOfSpotLights_, numberOfSpotLights);
    glUniform1i(shaders_[shaderID]->numberOfDirectionalLights_, numberOfDirectionalLights);

    qDebug() << "number of point Lights: "<< numberOfPointLights;
}

void ShaderManager::TransmitUniformLightDataToShader(size_t shaderID, gsl::Vector3D position, std::shared_ptr<LightComponent> light)
{
    glUseProgram(shaders_[shaderID]->program_);
    switch (light->lightType_)
    {
    case POINT_LIGHT:
        glUniform3f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].position, position.x, position.y, position.z);
        glUniform3f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].ambient, light->ambient_.x / gsl::MAX_COLORS, light->ambient_.y / gsl::MAX_COLORS, light->ambient_.z / gsl::MAX_COLORS);
        glUniform3f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].diffuse, light->diffuse_.x / gsl::MAX_COLORS, light->diffuse_.y / gsl::MAX_COLORS, light->diffuse_.z / gsl::MAX_COLORS);
        glUniform3f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].specular, light->specular_.x / gsl::MAX_COLORS, light->specular_.y / gsl::MAX_COLORS, light->specular_.z / gsl::MAX_COLORS);

        glUniform1f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].constant, light->constant_);
        glUniform1f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].linear, light->linear_);
        glUniform1f(shaders_[shaderID]->pointLight_[light->lightIndexForShader_].quadratic, light->quadratic_);
        break;
    case DIRECTIONAL_LIGHT:
        glUniform3f(shaders_[shaderID]->dirLight_[light->lightIndexForShader_].ambient, light->ambient_.x / gsl::MAX_COLORS, light->ambient_.y / gsl::MAX_COLORS, light->ambient_.z / gsl::MAX_COLORS);
        glUniform3f(shaders_[shaderID]->dirLight_[light->lightIndexForShader_].diffuse, light->diffuse_.x / gsl::MAX_COLORS, light->diffuse_.y / gsl::MAX_COLORS, light->diffuse_.z / gsl::MAX_COLORS);
        glUniform3f(shaders_[shaderID]->dirLight_[light->lightIndexForShader_].specular, light->specular_.x / gsl::MAX_COLORS, light->specular_.y / gsl::MAX_COLORS, light->specular_.z / gsl::MAX_COLORS);

        glUniform3f(shaders_[shaderID]->dirLight_[light->lightIndexForShader_].direction, light->direction_.x, light->direction_.y, light->direction_.z);
        break;
    case SPOT_LIGHT:
        glUniform3f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].position, position.x, position.y, position.z);
        glUniform3f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].ambient, light->ambient_.x / gsl::MAX_COLORS, light->ambient_.y / gsl::MAX_COLORS, light->ambient_.z / gsl::MAX_COLORS);
        glUniform3f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].diffuse, light->diffuse_.x / gsl::MAX_COLORS, light->diffuse_.y / gsl::MAX_COLORS, light->diffuse_.z / gsl::MAX_COLORS);
        glUniform3f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].specular, light->specular_.x / gsl::MAX_COLORS, light->specular_.y / gsl::MAX_COLORS, light->specular_.z / gsl::MAX_COLORS);

        glUniform1f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].cutOff, light->cutOff_);
        glUniform1f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].outerCutOff, light->outerCutOff_);

        glUniform3f(shaders_[shaderID]->spotLight_[light->lightIndexForShader_].direction, light->direction_.x, light->direction_.y, light->direction_.z);
        break;
    }

}
