#include "meshmanager.h"
#include <QFile>

MeshManager::MeshManager()
{
    qDebug() << "\n\nINITIALIZING MESH MANAGER";
    std::pair <std::vector<Vertex>,std::vector<GLuint>> data;

    data = readOBJFile((gsl::meshFilePath + "Alberto_Camera.obj").toStdString());
    cameraMesh_ = std::make_shared<Mesh>("Alberto_Camera.obj", FILE_MESH, makeCollisionBox(data.first));
    UpdateMesh(cameraMesh_, data.first, 0, data.second);

    // Meshes that cannot be deleted
    AddMesh(FILE_MESH, gsl::meshFilePath + "Alberto_Box.txt"); // 0
    AddMesh(SKYBOX_MESH); // 1
}

void MeshManager::AddMesh(MeshType meshType, QString filePath)
{
    std::pair <std::vector<Vertex>,std::vector<GLuint>> data;

    switch (meshType)
    {
    case XYZ_MESH:
        //        AddNewMesh(filePath, "XYZ", XYZ_MESH, XYZ());
        break;
    case SPHERE_MESH:
        //        AddNewMesh(filePath, "Sphere", SPHERE_MESH, Octahedron(4));
        //        AddNewMesh(filePath, "Sphere", SPHERE_MESH, Octahedron(2), 1);
        //        AddNewMesh(filePath, "Sphere", SPHERE_MESH, Octahedron(1), 2);
        break;
    case SKYBOX_MESH:
        data = SkyBox();
        meshes_.push_back(std::make_shared<Mesh>("Skybox", meshType, makeCollisionBox(data.first)));
        UpdateMesh(meshes_.size() - 1, data.first, 0, data.second);
        break;
    case FILE_MESH:
    {
        QString fileName = filePath.section('/', -1);

        if(fileName == "Skybox")
        {
            data = SkyBox();
            meshes_.push_back(std::make_shared<Mesh>("Skybox", meshType, makeCollisionBox(data.first)));
            UpdateMesh(meshes_.size() - 1, data.first, 0, data.second);
            return;
        }


        QString fileExtension = filePath.section('.', -1).toLower();

        if(fileExtension == "obj")
            data = readOBJFile(filePath.toStdString());
        else if(fileExtension == "txt")
            data.first = ReadTXTFile(filePath);
        else
            return;

        meshes_.push_back(std::make_shared<Mesh>(fileName, meshType, makeCollisionBox(data.first)));

        if(QFile::exists(filePath))
            UpdateMesh(meshes_.size() - 1, data.first, 0, data.second);

        QString lod1filePath = filePath;
        lod1filePath.insert(lod1filePath.lastIndexOf("."), "_L01");

        if(QFile::exists(lod1filePath))
        {
            if(fileExtension == "obj")
                data = readOBJFile(lod1filePath.toStdString());
            else if(fileExtension == "txt")
                data.first = ReadTXTFile(lod1filePath);
            UpdateMesh(meshes_.size() - 1, data.first, 1, data.second);
        }
        QString lod2filePath = filePath;
        lod2filePath.insert(lod2filePath.lastIndexOf("."), "_L02");
        if(QFile::exists(lod2filePath))
        {
            if(fileExtension == "obj")
                data = readOBJFile(lod2filePath.toStdString());
            else if(fileExtension == "txt")
                data.first = ReadTXTFile(lod2filePath);
            UpdateMesh(meshes_.size() - 1, data.first, 2, data.second);
        }
        break;
    }
    }
}

std::pair <std::vector<Vertex>,std::vector<GLuint>> MeshManager::readOBJFile(std::string fileWithPath)
{

    QString fileName = QString::fromStdString(fileWithPath).section('/', -1);
    if(!QFile::exists(gsl::meshFilePath + fileName))
    {
        qDebug() << "File copying from" << QString::fromStdString(fileWithPath) << "to" << (gsl::meshFilePath + fileName);
        QFile::copy(QString::fromStdString(fileWithPath), gsl::meshFilePath + fileName);

    }
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    std::ifstream fileIn;
    fileIn.open (fileWithPath, std::ifstream::in);
    if(!fileIn)
        qDebug() << "Could not open file for reading: " << QString::fromStdString(fileWithPath);

    //One line at a time-variable
    std::string oneLine;
    //One word at a time-variable
    std::string oneWord;

    std::vector<gsl::Vector3D> tempVertecies;
    std::vector<gsl::Vector3D> tempNormals;
    std::vector<gsl::Vector2D> tempUVs;

    // Varible for constructing the indices vector
    unsigned int temp_index = 0;

    //Reading one line at a time from file to oneLine
    while(std::getline(fileIn, oneLine))
    {
        //Doing a trick to get one word at a time
        std::stringstream sStream;
        //Pushing line into stream
        sStream << oneLine;
        //Streaming one word out of line
        oneWord = ""; //resetting the value or else the last value might survive!

        sStream >> oneWord;

        if (oneWord == "#")
            continue;
        if (oneWord == "")
            continue;

        if (oneWord == "v")
        {
            gsl::Vector3D tempVertex;
            sStream >> oneWord;
            tempVertex.x = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.y = std::stof(oneWord);
            sStream >> oneWord;
            tempVertex.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempVertecies.push_back(tempVertex);

            continue;
        }
        if (oneWord == "vt")
        {
            //            qDebug() << "Line is UV-coordinate "  << QString::fromStdString(oneWord) << " ";
            gsl::Vector2D tempUV;
            sStream >> oneWord;
            tempUV.x = std::stof(oneWord);
            sStream >> oneWord;
            tempUV.y = std::stof(oneWord);

            //UV made - pushing it into UV-vector
            tempUVs.push_back(tempUV);

            continue;
        }
        if (oneWord == "vn")
        {
            gsl::Vector3D tempNormal;
            sStream >> oneWord;
            tempNormal.x = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.y = std::stof(oneWord);
            sStream >> oneWord;
            tempNormal.z = std::stof(oneWord);

            //Vertex made - pushing it into vertex-vector
            tempNormals.push_back(tempNormal);
            continue;
        }
        if (oneWord == "f")
        {
            //int slash; //used to get the / from the v/t/n - format
            int index, normal, uv;
            for(size_t i = 0; i < 3; i++)
            {
                sStream >> oneWord;     //one word read
                std::stringstream tempWord(oneWord);    //to use getline on this one word
                std::string segment;    //the numbers in the f-line
                std::vector<std::string> segmentArray;  //temp array of the numbers
                while(std::getline(tempWord, segment, '/')) //splitting word in segments
                {
                    segmentArray.push_back(segment);
                }
                index = std::stoi(segmentArray[0]);     //first is vertex
                if (segmentArray[1] != "")              //second is uv
                    uv = std::stoi(segmentArray[1]);
                else
                {
                    //qDebug() << "No uvs in mesh";       //uv not present
                    uv = 0;                             //this will become -1 in a couple of lines
                }
                normal = std::stoi(segmentArray[2]);    //third is normal

                //Fixing the indexes
                //because obj f-lines starts with 1, not 0
                --index;
                --uv;
                --normal;

                if (uv > -1)    //uv present!
                {
                    Vertex tempVert(tempVertecies[static_cast<size_t>(index)], tempNormals[static_cast<size_t>(normal)], tempUVs[static_cast<size_t>(uv)]);
                    vertices.push_back(tempVert);
                }
                else            //no uv in mesh data, use 0, 0 as uv
                {
                    Vertex tempVert(tempVertecies[static_cast<size_t>(index)], tempNormals[static_cast<size_t>(normal)], gsl::Vector2D(0.0f, 0.0f));
                    vertices.push_back(tempVert);
                }
                indices.push_back(temp_index++);
            }

            //            continue;
        }
    }

    //beeing a nice boy and closing the file after use
    fileIn.close();
    qDebug() << "Obj file read: " << QString::fromStdString(fileWithPath);

    return std::make_pair(vertices, indices);

}

void MeshManager::UpdateMesh(size_t meshID, std::vector<Vertex> vertices, int lodLevel, std::vector<GLuint> indices)
{
    UpdateMesh(meshes_[meshID], vertices, lodLevel, indices);
}

void MeshManager::UpdateMesh(std::shared_ptr<Mesh> mesh, std::vector<Vertex> vertices, int lodLevel, std::vector<GLuint> indices)
{
    mesh->numberOfVertices_[lodLevel] = vertices.size();
    mesh->numberOfIndices_[lodLevel] = indices.size();

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mesh->VAO_[lodLevel]);
    glBindVertexArray( mesh->VAO_[lodLevel]);

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mesh->VBO_[lodLevel] );
    glBindBuffer( GL_ARRAY_BUFFER, mesh->VBO_[lodLevel] );
    glBufferData( GL_ARRAY_BUFFER, static_cast<int>(mesh->numberOfVertices_[lodLevel]*sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), static_cast<GLvoid*>(nullptr));
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex), reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof( Vertex ), reinterpret_cast<GLvoid*>( 6 * sizeof( GLfloat ) ));
    glEnableVertexAttribArray(2);

    if (mesh->numberOfIndices_[lodLevel] > 0)
    {
        //Second buffer - holds the indices (Element Array Buffer - EAB):
        glGenBuffers(1, &mesh->EAB_[lodLevel]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EAB_[lodLevel]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(mesh->numberOfIndices_[lodLevel] * sizeof(GLuint)), indices.data(), GL_STATIC_DRAW);
    }

    // BOUNDING BOX TEST

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mesh->boundingBox_->VAO_);
    glBindVertexArray( mesh->boundingBox_->VAO_);

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mesh->boundingBox_->VBO_ );
    glBindBuffer( GL_ARRAY_BUFFER, mesh->boundingBox_->VBO_ );
    glBufferData( GL_ARRAY_BUFFER, static_cast<int>(mesh->boundingBox_->points_.size()*sizeof(gsl::Vector3D)), mesh->boundingBox_->points_.data(), GL_STATIC_DRAW );

    // 1rst attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(gsl::Vector3D), static_cast<GLvoid*>(nullptr));
    glEnableVertexAttribArray(0);

    // indices
    glGenBuffers(1, &mesh->boundingBox_->EAB_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->boundingBox_->EAB_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(mesh->boundingBox_->indices_.size() * sizeof(GLuint)), mesh->boundingBox_->indices_.data(), GL_STATIC_DRAW);

    qDebug() << "VAO: " << mesh->VAO_[lodLevel] << "bb VAO: " << mesh->boundingBox_->VAO_ << " VBO: " << mesh->VBO_[lodLevel] << " EAB: " << mesh->EAB_[lodLevel] << "bb VBO: " << mesh->boundingBox_->VBO_;
    glBindVertexArray(0);
}

void MeshManager::DeleteMesh(std::shared_ptr<Mesh> mesh)
{
    for(int lodLevel = 0; lodLevel < 3; lodLevel++)
    {
        if (mesh->VAO_[lodLevel] == 0)
            continue;

        //must call this to use OpenGL functions
        initializeOpenGLFunctions();

        //Vertex Array Object - VAO
        glDeleteVertexArrays(1, &mesh->VAO_[lodLevel]);

        //Vertex Buffer Object to hold vertices - VBO
        glDeleteBuffers( 1, &mesh->VBO_[lodLevel] );


        if (mesh->numberOfIndices_[lodLevel] > 0)
        {
            //Second buffer - holds the indices (Element Array Buffer - EAB):
            glDeleteBuffers(1, &mesh->EAB_[lodLevel]);
        }

        // BOUNDING BOX TEST

        //Vertex Array Object - VAO
        glDeleteVertexArrays( 1, &mesh->boundingBox_->VAO_);

        //Vertex Buffer Object to hold vertices - VBO
        glDeleteBuffers( 1, &mesh->boundingBox_->VBO_ );

        // indices
        glDeleteBuffers(1, &mesh->boundingBox_->EAB_);

        qDebug() << "Mesh deleted" << mesh->name_ << "Lod level " << lodLevel << "VAO: " << mesh->VAO_[lodLevel] << "bb VAO: " << mesh->boundingBox_->VAO_ << " VBO: " << mesh->VBO_[lodLevel] << " EAB: " << mesh->EAB_[lodLevel] << "bb VBO: " << mesh->boundingBox_->VBO_;
    }
}

void MeshManager::DeleteMesh(size_t meshID)
{
    DeleteMesh(meshes_[meshID]);
    //TODO MAKE SURE THE THINGS GET DELETED IN THE OPENGL THANG YO
    meshes_.erase(meshes_.begin() + meshID);
}

void MeshManager::DeleteAllCustomMeshes()
{
    for (size_t i = meshes_.size() - 1; i >= numberOfDefaultMeshes_; i--)
    {
        DeleteMesh(i);
    }
}

std::vector<Vertex> MeshManager::ReadTXTFile(QString filePath) {

    QString fileName = filePath.section('/', -1);
    if(!QFile::exists(gsl::meshFilePath + fileName))
        QFile::copy(filePath, gsl::meshFilePath + fileName);

    std::vector<Vertex> vertices;

    std::ifstream in;
    //    QString fileWithPath = gsl::assetFilePath + "Meshes/" + filePath;

    in.open(filePath.toStdString());

    if (in.is_open()) {
        size_t n;
        Vertex vertex;
        in >> n;
        vertices.reserve(n);
        for (size_t i = 0; i < n; i++) {
            in >> vertex;
            vertices.push_back(vertex);
        }
        in.close();
        qDebug() << "TriangleSurface file read: " << filePath;
    }
    else
    {
        qDebug() << "Could not open file for reading: " << filePath;
    }

    return vertices;
}

void MeshManager::WriteVertexDataToFile(std::vector<Vertex> vertices, std::string filename)
{
    std::ofstream out;
    out.open(filename.c_str());

    if (out.is_open())
    {
        size_t n = vertices.size();
        Vertex vertex;
        out << n << std::endl;
        for (auto it=vertices.begin(); it != vertices.end(); it++)
        {
            vertex = *it;
            out << vertex << std::endl;
        }
        out.close();
    }
}

std::vector<Vertex> MeshManager::XYZ()
{
    std::vector<Vertex> vertices;
    vertices.push_back(Vertex{0.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    vertices.push_back(Vertex{100.f, 0.f, 0.f, 1.f, 0.f, 0.f});
    vertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 1.f, 0.f});
    vertices.push_back(Vertex{0.f, 100.f, 0.f, 0.f, 1.f, 0.f});
    vertices.push_back(Vertex{0.f, 0.f, 0.f, 0.f, 0.f, 1.f});
    vertices.push_back(Vertex{0.f, 0.f, 100.f, 0.f, 0.f, 1.f});
    qDebug() << "XYZ vertex data generated";
    return vertices;
}

std::vector<Vertex> MeshManager::Octahedron(int recursions)
{
    std::vector<Vertex> vertices;

    gsl::Vector3D v0(0.f, 0.f, 1.f);
    gsl::Vector3D v1(1.f, 0.f, 0.f);
    gsl::Vector3D v2(0.f, 1.f, 0.f);
    gsl::Vector3D v3(-1.f, 0.f, 0.f);
    gsl::Vector3D v4(0.f, -1.f, 0.f);
    gsl::Vector3D v5(0.f, 0.f, -1.f);

    subDivide(vertices, v0, v1, v2, recursions);
    subDivide(vertices, v0, v2, v3, recursions);
    subDivide(vertices, v0, v3, v4, recursions);
    subDivide(vertices, v0, v4, v1, recursions);
    subDivide(vertices, v5, v2, v1, recursions);
    subDivide(vertices, v5, v3, v2, recursions);
    subDivide(vertices, v5, v4, v3, recursions);
    subDivide(vertices, v5, v1, v4, recursions);

    qDebug() << "Octahedron ball vertex data generated";
    return vertices;
}

void MeshManager::subDivide(std::vector<Vertex>& verticesPtr, const gsl::Vector3D& a, const gsl::Vector3D& b, const gsl::Vector3D& c, GLint n)
{
    if (n > 0)
    {
        gsl::Vector3D v1 = a + b;
        v1.normalize();

        gsl::Vector3D v2 = a + c;
        v2.normalize();

        gsl::Vector3D v3 = c + b;
        v3.normalize();

        subDivide(verticesPtr, a, v1, v2, n-1);
        subDivide(verticesPtr, c, v2, v3, n-1);
        subDivide(verticesPtr, b, v3, v1, n-1);
        subDivide(verticesPtr, v3, v2, v1, n-1);
    }
    else
    {
        makeTriangle(verticesPtr, a, b, c);
    }
}

void MeshManager::makeTriangle(std::vector<Vertex>& verticesPtr, const gsl::Vector3D &v1, const gsl::Vector3D &v2, const gsl::Vector3D &v3)
{
    verticesPtr.push_back(Vertex(v1, v1, gsl::Vector2D(0.f, 0.f)));
    verticesPtr.push_back(Vertex(v2, v2, gsl::Vector2D(1.f, 0.f)));
    verticesPtr.push_back(Vertex(v3, v3, gsl::Vector2D(0.5f, 1.f)));

}

std::shared_ptr<BoundingBox> MeshManager::makeCollisionBox(std::vector<Vertex> vertices)
{
    float xmax = vertices.back().XYZ_.x;
    float xmin = vertices.back().XYZ_.x;
    float ymax = vertices.back().XYZ_.y;
    float ymin = vertices.back().XYZ_.y;
    float zmax = vertices.back().XYZ_.z;
    float zmin = vertices.back().XYZ_.z;
    for(size_t i = 1; i < vertices.size(); i++)
    {
        if(xmax < vertices[i].XYZ_.x)
            xmax = vertices[i].XYZ_.x;
        if(xmin > vertices[i].XYZ_.x)
            xmin = vertices[i].XYZ_.x;

        if(ymax < vertices[i].XYZ_.y)
            ymax = vertices[i].XYZ_.y;
        if(ymin > vertices[i].XYZ_.y)
            ymin = vertices[i].XYZ_.y;

        if(zmax < vertices[i].XYZ_.z)
            zmax = vertices[i].XYZ_.z;
        if(zmin > vertices[i].XYZ_.z)
            zmin = vertices[i].XYZ_.z;
    }
    std::shared_ptr<BoundingBox> boundingBox = std::make_shared<BoundingBox>();

    boundingBox->points_.push_back(gsl::Vector3D(xmax,ymax,zmax)); // 0
    boundingBox->points_.push_back(gsl::Vector3D(xmax,ymax,zmin)); // 1
    boundingBox->points_.push_back(gsl::Vector3D(xmin,ymax,zmax)); // 2
    boundingBox->points_.push_back(gsl::Vector3D(xmin,ymax,zmin)); // 3
    //Bottom
    boundingBox->points_.push_back(gsl::Vector3D(xmax,ymin,zmax)); // 4
    boundingBox->points_.push_back(gsl::Vector3D(xmax,ymin,zmin)); // 5
    boundingBox->points_.push_back(gsl::Vector3D(xmin,ymin,zmax)); // 6
    boundingBox->points_.push_back(gsl::Vector3D(xmin,ymin,zmin)); // 7
    return boundingBox;
}

std::pair<std::vector<Vertex>, std::vector<GLuint> > MeshManager::updateLandscapeMesh(QString fileWithPath)
{
    QString fileName = fileWithPath.section('/', -1);
    std::pair<std::vector<Vertex>,std::vector<GLuint>> data;
    data = readOBJFile(fileWithPath.toStdString());
    if(!landscapeMesh_)
        landscapeMesh_ = std::make_shared<Mesh>(fileName, FILE_MESH, makeCollisionBox(data.first));
    UpdateMesh(landscapeMesh_, data.first, 0, data.second);
    return data;
}

std::pair <std::vector<Vertex>,std::vector<GLuint>> MeshManager::SkyBox()
{
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;

    mVertices.insert( mVertices.end(),
    {
                          Vertex{gsl::Vector3D(-1.f, -1.f, 1.f),    gsl::Vector3D(0.f, 0.f, 1.0f),  gsl::Vector2D(0.25f, 0.333f)},  //v0
                          Vertex{gsl::Vector3D( 1.f, -1.f, 1.f),    gsl::Vector3D(0.f, 0.f, 1.0f),  gsl::Vector2D(0.5f,  0.333f)},  //v1
                          Vertex{gsl::Vector3D(-1.f,  1.f, 1.f),    gsl::Vector3D(0.f, 0.f, 1.0f),  gsl::Vector2D(0.25f, 0.666f)},    //v2
                          Vertex{gsl::Vector3D( 1.f,  1.f, 1.f),    gsl::Vector3D(0.f, 0.f, 1.0f),  gsl::Vector2D(0.5f,  0.666f)},    //v3

                          //Vertex data for right
                          Vertex{gsl::Vector3D(1.f, -1.f,  1.f),    gsl::Vector3D(1.f, 0.f, 0.f),   gsl::Vector2D(0.5f,  0.333f)},  //v4
                          Vertex{gsl::Vector3D(1.f, -1.f, -1.f),    gsl::Vector3D(1.f, 0.f, 0.f),   gsl::Vector2D(0.75f, 0.333f)},  //v5
                          Vertex{gsl::Vector3D(1.f,  1.f,  1.f),    gsl::Vector3D(1.f, 0.f, 0.f),   gsl::Vector2D(0.5f,  0.666f)},    //v6
                          Vertex{gsl::Vector3D(1.f,  1.f, -1.f),    gsl::Vector3D(1.f, 0.f, 0.f),   gsl::Vector2D(0.75f, 0.666f)},    //v7

                          //Vertex data for back
                          Vertex{gsl::Vector3D( 1.f, -1.f, -1.f),   gsl::Vector3D(0.f, 0.f, -1.f),  gsl::Vector2D(0.75f, 0.333f)},  //v8
                          Vertex{gsl::Vector3D(-1.f, -1.f, -1.f),   gsl::Vector3D(0.f, 0.f, -1.f),  gsl::Vector2D(1.f,   0.333f)},  //v9
                          Vertex{gsl::Vector3D( 1.f,  1.f, -1.f),   gsl::Vector3D(0.f, 0.f, -1.f),  gsl::Vector2D(0.75f, 0.666f)},    //v10
                          Vertex{gsl::Vector3D(-1.f,  1.f, -1.f),   gsl::Vector3D(0.f, 0.f, -1.f),  gsl::Vector2D(1.f,   0.666f)},    //v11

                          //Vertex data for left
                          Vertex{gsl::Vector3D(-1.f, -1.f, -1.f),   gsl::Vector3D(-1.f, 0.f, 0.f),  gsl::Vector2D(0.f, 0.333f)},    //v12
                          Vertex{gsl::Vector3D(-1.f, -1.f,  1.f),   gsl::Vector3D(-1.f, 0.f, 0.f),  gsl::Vector2D(0.25f, 0.333f)},  //v13
                          Vertex{gsl::Vector3D(-1.f,  1.f, -1.f),   gsl::Vector3D(-1.f, 0.f, 0.f),  gsl::Vector2D(0.f, 0.666f)},      //v14
                          Vertex{gsl::Vector3D(-1.f,  1.f,  1.f),   gsl::Vector3D(-1.f, 0.f, 0.f),  gsl::Vector2D(0.25f,   0.666f)},  //v15

                          //Vertex data for bottom
                          Vertex{gsl::Vector3D(-1.f, -1.f, -1.f),   gsl::Vector3D(0.f, -1.f, 0.f),  gsl::Vector2D(0.25f, 0.f)},      //v16
                          Vertex{gsl::Vector3D( 1.f, -1.f, -1.f),   gsl::Vector3D(0.f, -1.f, 0.f),  gsl::Vector2D(0.5f,  0.f)},      //v17
                          Vertex{gsl::Vector3D(-1.f, -1.f,  1.f),   gsl::Vector3D(0.f, -1.f, 0.f),  gsl::Vector2D(0.25f, 0.333f)},   //v18
                          Vertex{gsl::Vector3D( 1.f, -1.f,  1.f),   gsl::Vector3D(0.f, -1.f, 0.f),  gsl::Vector2D(0.5f,  0.333f)},   //v19

                          //Vertex data for top
                          Vertex{gsl::Vector3D(-1.f, 1.f,  1.f),    gsl::Vector3D(0.f, 1.f, 0.f),   gsl::Vector2D(0.25f, 0.666f)},    //v20
                          Vertex{gsl::Vector3D( 1.f, 1.f,  1.f),    gsl::Vector3D(0.f, 1.f, 0.f),   gsl::Vector2D(0.5f,  0.666f)},    //v21
                          Vertex{gsl::Vector3D(-1.f, 1.f, -1.f),    gsl::Vector3D(0.f, 1.f, 0.f),   gsl::Vector2D(0.25f, 0.999f)},      //v22
                          Vertex{gsl::Vector3D( 1.f, 1.f, -1.f),    gsl::Vector3D(0.f, 1.f, 0.f),   gsl::Vector2D(0.5f,  0.999f)}       //v23
                      });

    mIndices.insert( mIndices.end(),
    { 0,  2,  1,  1,  2,  3,      //Face 0 - triangle strip (v0,  v1,  v2,  v3)
      4,  6,  5,  5,  6,  7,      //Face 1 - triangle strip (v4,  v5,  v6,  v7)
      8,  10,  9, 9, 10, 11,      //Face 2 - triangle strip (v8,  v9, v10,  v11)
      12, 14, 13, 13, 14, 15,     //Face 3 - triangle strip (v12, v13, v14, v15)
      16, 18, 17, 17, 18, 19,     //Face 4 - triangle strip (v16, v17, v18, v19)
      20, 22, 21, 21, 22, 23      //Face 5 - triangle strip (v20, v21, v22, v23)
                     });
    qDebug() << "Skybox vertex data generated";
    return std::make_pair(mVertices, mIndices);
};
