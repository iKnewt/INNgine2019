#ifndef MESHMANAGER_H
#define MESHMANAGER_H

class Shader;
class Texture;

/// Contains BoundingBox data
struct BoundingBox
{
    /// Vector3D of the 8 corners of a cube, usually generated based on mesh.
    std::vector<gsl::Vector3D> points_;
    /// Indices used to render the corner points as cube.
    std::vector<GLuint> indices_
    {
        0, 1, 0, 2, 0, 4,
        3, 2, 3, 1, 3, 7,
        5, 1, 5, 4, 5, 7,
        6, 2, 6, 4, 6, 7
    };
    
    /// Vertex Array Object.
    uint VAO_{0};
    /// Vertex Buffer Object.
    uint VBO_{0};
    /// Element Array Buffer.
    uint EAB_{0};
};

/// Enum that defines how to generate the vertex data for the mesh.
enum MeshType
{
    XYZ_MESH,
    FILE_MESH,
    SPHERE_MESH,
    SKYBOX_MESH
};

/// Contains mesh data.
struct Mesh
{
    Mesh(QString name, MeshType meshType, std::shared_ptr<BoundingBox> boundingBox) : name_(name), meshType_(meshType), boundingBox_(boundingBox) {}
    
    /// Name of mesh, used to identify mesh in GUI and debug.
    QString name_;
    /// Defines how a mesh was generated.
    /// Used to define some exceptions when rendering specific mesh types.
    MeshType meshType_;
    
    /// Vertex Array Object, one for each LOD.
    /// Stores info on how to interpret the data in the VBO.
    uint VAO_[3]{0};
    /// Vertex Buffer Object, one for each LOD.
    /// Stores the actual data on how to render the mesh, such as vertices.
    uint VBO_[3]{0};
    /// Element Array Buffer, one for each LOD.
    /// Optional, stores the order of vertices in indiced mesh.
    uint EAB_[3]{0};
    
    /// Total number of vertices, one for each LOD.
    /// Ussed with glDrawArrays.
    size_t numberOfVertices_[3]{0};
    /// Total number of indices, one for each LOD.
    /// Used with glDrawElements.
    size_t numberOfIndices_[3]{0};
    
    /// The bounding box of the mesh.
    std::shared_ptr<BoundingBox> boundingBox_;
};

/// Keeps all the data and logic connected to Meshes.
class MeshManager : public QOpenGLFunctions_4_1_Core
{
public:
    MeshManager();
    /// Number of defaults, used to make sure default are not deleted or edited.
    const size_t numberOfDefaultMeshes_{2};
    /// Mesh for all cameras
    std::shared_ptr<Mesh> cameraMesh_{nullptr};
    /// Mesh for landscape
    std::shared_ptr<Mesh> landscapeMesh_{nullptr};
    /// Vector containing all meshes.
    std::vector<std::shared_ptr<Mesh>> meshes_;

    /**
     * Reads vertex data from OBJ file.
     * @param filePath FilePath of obj file to read from.
     * @return Vertex data read from obj file.
     */
    static std::pair<std::vector<Vertex>, std::vector<GLuint> > readOBJFile(std::string fileWithPath);
    /**
     * Adds single mesh to meshes_.
     * @param meshType How to read the mesh.
     * @param filePath Filepath or filename(if placed in gsl::assetFilePath + "Meshes/" folder) of mesh data.
     * @return The newly created mesh, or an old one of duplicate found.
     */
    void AddMesh(MeshType meshType, QString filePath = "");
    /**
     * Reads vertex data from TXT file.
     * @param filePath FilePath of txt file to read from.
     * @return Vertex data read from txt file.
     */
    std::vector<Vertex> ReadTXTFile(QString filePath);
    /**
     * Deletes mesh from meshes_ and calls to delete from openGL
     * @param meshID ID of mesh to delete.
     */
    void DeleteMesh(size_t meshID);
    /**
     * Deletes all meshes added after the defaults.
     */
    void DeleteAllCustomMeshes();
    /**
     * Updates landscape mesh.
     * @param fileWithPath
     * @return the new vertex and indice data to be used with barycentric coordinates.
     */
    std::pair<std::vector<Vertex>, std::vector<GLuint> > updateLandscapeMesh(QString fileWithPath);

private:
    /**
     * Deletes mesh from openGL
     * @param mesh mesh to delete
     */
    void DeleteMesh(std::shared_ptr<Mesh> mesh);
    /**
     * Update or override existing mesh.
     * @param meshIndex Index of mesh to update.
     * @param vertices New Vertex data.
     * @param lodLevel Which lodLevel to update.
     * @param indices Optional, New Indice data.
     */
    void UpdateMesh(size_t meshID, std::vector<Vertex> vertices, int lodLevel = 0, std::vector<GLuint> indices = std::vector<GLuint>());
    /**
     * Update or override existing mesh.
     * @param mesh
     * @param vertices New Vertex data.
     * @param lodLevel Which lodLevel to update.
     * @param indices Optional, New Indice data.
     */
    void UpdateMesh(std::shared_ptr<Mesh> mesh, std::vector<Vertex> vertices, int lodLevel = 0, std::vector<GLuint> indices = std::vector<GLuint>());
    /**
     * Creates SkyBox vertex and indice data.
     * @return Vertex and Indices for a SkyBox Mesh.
     */
    std::pair<std::vector<Vertex>, std::vector<GLuint> > SkyBox();
    /**
     * Creates vertex data for an axis XYZ cross pointing 100 in each direction.
     * @return Vertex data for an axis cross pointing 100 in each direction.
     */
    std::vector<Vertex> XYZ();
    /**
     * Creates vertex data for an octahedronball.
     * @param recursions How many recursions to use, affects the level of detail of the ball.
     * @return Vertex data for a octahedron ball.
     */
    std::vector<Vertex> Octahedron(int recursions = 2);
    /**
     * Used to make Octahedron ball.
     * @param a
     * @param b
     * @param c
     * @param n
     */
    void subDivide(std::vector<Vertex>&, const gsl::Vector3D& a, const gsl::Vector3D& b, const gsl::Vector3D& c, GLint n);
    /**
     * Used to make Octahedron ball.
     * @param v1
     * @param v2
     * @param v3
     */
    void makeTriangle(std::vector<Vertex>&, const gsl::Vector3D& v1, const gsl::Vector3D& v2, const gsl::Vector3D& v3);
    /**
     * Generates Vertex data for a collision box based on input vertices.
     * Calling this function happens in CreateNewMesh()
     * @param vertices Used to calculate bounding Box corners.
     * @return The generated BoundingBox.
     */
    std::shared_ptr<BoundingBox> makeCollisionBox(std::vector<Vertex> vertices);

    void WriteVertexDataToFile(std::vector<Vertex> mVertices, std::string filename);
};

#endif // MESHMANAGER_H
