#include "GLSLloader.hpp"

using namespace glm;

void compileShader(GLuint shader_ID, std::string shader_code)
{
    GLint result = GL_FALSE;
    int InfoLogLength;

    // Compile shader
    char const * SourcePointer = shader_code.c_str();

    glShaderSource(shader_ID, 1, &SourcePointer, nullptr);
    glCompileShader(shader_ID);

    // Check shader
    glGetShaderiv(shader_ID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_ID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if(InfoLogLength > 0)
    {
        std::vector<char> ShaderErrorMessage(InfoLogLength + 1);

        glGetShaderInfoLog(shader_ID, InfoLogLength, nullptr, &ShaderErrorMessage[0]);
        throw std::runtime_error(&ShaderErrorMessage[0]);
    }
}

GLuint linkProgram(GLuint vertex_shader_ID, GLuint fragment_shader_ID)
{
    GLint result = GL_FALSE;
    int InfoLogLength;

    // Link the program
    GLuint ProgramID = glCreateProgram();

    std::cout << "Linking program\n";
    glAttachShader(ProgramID, vertex_shader_ID);
    glAttachShader(ProgramID, fragment_shader_ID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if(InfoLogLength > 0)
    {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);

        glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        std::cout << &ProgramErrorMessage[0] << "\n";
    }

    return ProgramID;
}

GLuint prepareShader(const std::string & file_path, GLenum shader_type)
{
    // Read the shader code from the file
    std::string shader_code;
    std::ifstream shaderStream(file_path, std::ios::in);

    if(shaderStream.is_open())
    {
        std::string Line = "";

        while(getline(shaderStream, Line))
            shader_code += "\n" + Line;

        shaderStream.close();
    }
    else
        throw std::runtime_error(std::string("Impossible to open ") + file_path);

    GLuint shader_ID = glCreateShader(shader_type);

    std::cout << "Compiling shader : " << file_path << "\n";
    compileShader(shader_ID, shader_code);

    return shader_ID;
}

std::tuple<GLuint, GLuint> loadShaders(const std::string & area_vertex_file_path,
                                       const std::string & earth_vertex_file_path,
                                       const std::string & fragment_file_path)
{
    // Create and compile shaders
    GLuint Areavertex_shader_ID = prepareShader(area_vertex_file_path, GL_VERTEX_SHADER);
    GLuint Earthvertex_shader_ID = prepareShader(earth_vertex_file_path, GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = prepareShader(fragment_file_path, GL_FRAGMENT_SHADER);

    GLuint areaProgramID = linkProgram(Areavertex_shader_ID, fragment_shader_ID);
    GLuint earthProgramID = linkProgram(Earthvertex_shader_ID, fragment_shader_ID);

    glDetachShader(areaProgramID, Areavertex_shader_ID);
    glDetachShader(areaProgramID, fragment_shader_ID);
    glDetachShader(earthProgramID, Earthvertex_shader_ID);
    glDetachShader(earthProgramID, fragment_shader_ID);

    glDeleteShader(Areavertex_shader_ID);
    glDeleteShader(Earthvertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    std::cout << "Shaders loaded!\n";

    return std::make_tuple(areaProgramID, earthProgramID);
}
