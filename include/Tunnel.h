//
// Created by USER on 14/06/2018.
//

#ifndef FCG_TUNNEL_H
#define FCG_TUNNEL_H


#include <glad/glad.h>
#include "fcg_util.hpp"
#include "Shader.h"

class Tunnel {
public:
    GLuint tunnel_vao;

    Tunnel(){
        tunnel_vao = BuildTriangles();
    }

    void draw(Shader shader){
        glBindVertexArray(this->tunnel_vao);
        shader.activate();

        glm::mat4 model = Matrix_Identity();
        shader.passValue("model", model);
        DrawPlane(false);

        shader.deactivate();
        glBindVertexArray(0);
    }

private:
    // Função que desenha um cubo com arestas em preto, definido dentro da função BuildTriangles().
    void DrawPlane(GLint render_as_black_uniform)
    {
        // Informamos para a placa de vídeo (GPU) que a variável booleana
        // "render_as_black" deve ser colocada como "false". Veja o arquivo
        // "shader_vertex.glsl".
        glUniform1i(render_as_black_uniform, false);

        // Pedimos para a GPU rasterizar os vértices do cubo apontados pelo
        // VAO como triângulos, formando as faces do cubo. Esta
        // renderização irá executar o Vertex Shader definido no arquivo
        // "shader_vertex.glsl", e o mesmo irá utilizar as matrizes
        // "model", "view" e "projection" definidas acima e já enviadas
        // para a placa de vídeo (GPU).
        //
        // Veja a definição de g_VirtualScene["cube_faces"] dentro da
        // função BuildTriangles(), e veja a documentação da função
        // glDrawElements() em http://docs.gl/gl3/glDrawElements.
        glDrawElements(
            g_VirtualScene["wall"].rendering_mode, // Veja slide 175 do documento "Aula_04_Modelagem_Geometrica_3D.pdf".
            g_VirtualScene["wall"].num_indices,    //
            GL_UNSIGNED_INT,
            (void*)g_VirtualScene["wall"].first_index
        );
    }

    GLuint BuildTriangles()
    {
        // Primeiro, definimos os atributos de cada vértice.

        // A posição de cada vértice é definida por coeficientes em um sistema de
        // coordenadas local de cada modelo geométrico. Note o uso de coordenadas
        // homogêneas.  Veja as seguintes referências:
        //
        //  - slide 32  do documento "Aula_08_Sistemas_de_Coordenadas.pdf";
        //  - slide 147 do documento "Aula_08_Sistemas_de_Coordenadas.pdf";
        //  - slide 144 do documento "Aula_09_Projecoes.pdf".
        //
        // Este vetor "model_coefficients" define a GEOMETRIA (veja slide 112 do
        // documento "Aula_04_Modelagem_Geometrica_3D.pdf").
        //
        GLfloat model_coefficients[] = {
                // Vértices de um cubo
                //    X      Y     Z     W
                -1.0f,  1.0f,  0.0f, 1.0f, // posição do vértice 0
                -1.0f, -1.0f,  0.0f, 1.0f, // posição do vértice 1
                1.0f, -1.0f,  0.0f, 1.0f, // posição do vértice 2
                1.0f,  1.0f,  0.0f, 1.0f, // posição do vértice 3
        };

        // Criamos o identificador (ID) de um Vertex Buffer Object (VBO).  Um VBO é
        // um buffer de memória que irá conter os valores de um certo atributo de
        // um conjunto de vértices; por exemplo: posição, cor, normais, coordenadas
        // de textura.  Neste exemplo utilizaremos vários VBOs, um para cada tipo de atributo.
        // Agora criamos um VBO para armazenarmos um atributo: posição.
        GLuint VBO_model_coefficients_id;
        glGenBuffers(1, &VBO_model_coefficients_id);

        // Criamos o identificador (ID) de um Vertex Array Object (VAO).  Um VAO
        // contém a definição de vários atributos de um certo conjunto de vértices;
        // isto é, um VAO irá conter ponteiros para vários VBOs.
        GLuint vertex_array_object_id;
        glGenVertexArrays(1, &vertex_array_object_id);

        // "Ligamos" o VAO ("bind"). Informamos que iremos atualizar o VAO cujo ID
        // está contido na variável "vertex_array_object_id".
        glBindVertexArray(vertex_array_object_id);

        // "Ligamos" o VBO ("bind"). Informamos que o VBO cujo ID está contido na
        // variável VBO_model_coefficients_id será modificado a seguir. A
        // constante "GL_ARRAY_BUFFER" informa que esse buffer é de fato um VBO, e
        // irá conter atributos de vértices.
        glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);

        // Alocamos memória para o VBO "ligado" acima. Como queremos armazenar
        // nesse VBO todos os valores contidos no array "model_coefficients", pedimos
        // para alocar um número de bytes exatamente igual ao tamanho ("size")
        // desse array. A constante "GL_STATIC_DRAW" dá uma dica para o driver da
        // GPU sobre como utilizaremos os dados do VBO. Neste caso, estamos dizendo
        // que não pretendemos alterar tais dados (são estáticos: "STATIC"), e
        // também dizemos que tais dados serão utilizados para renderizar ou
        // desenhar ("DRAW").  Pense que:
        //
        //            glBufferData()  ==  malloc() do C  ==  new do C++.
        //
        glBufferData(GL_ARRAY_BUFFER, sizeof(model_coefficients), NULL, GL_STATIC_DRAW);

        // Finalmente, copiamos os valores do array model_coefficients para dentro do
        // VBO "ligado" acima.  Pense que:
        //
        //            glBufferSubData()  ==  memcpy() do C.
        //
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(model_coefficients), model_coefficients);

        // Precisamos então informar um índice de "local" ("location"), o qual será
        // utilizado no shader "shader_vertex.glsl" para acessar os valores
        // armazenados no VBO "ligado" acima. Também, informamos a dimensão (número de
        // coeficientes) destes atributos. Como em nosso caso são pontos em coordenadas
        // homogêneas, temos quatro coeficientes por vértice (X,Y,Z,W). Isso define
        // um tipo de dado chamado de "vec4" em "shader_vertex.glsl": um vetor com
        // quatro coeficientes. Finalmente, informamos que os dados estão em ponto
        // flutuante com 32 bits (GL_FLOAT).
        // Esta função também informa que o VBO "ligado" acima em glBindBuffer()
        // está dentro do VAO "ligado" acima por glBindVertexArray().
        // Veja https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Buffer_Object
        GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
        GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);

        // "Ativamos" os atributos. Informamos que os atributos com índice de local
        // definido acima, na variável "location", deve ser utilizado durante o
        // rendering.
        glEnableVertexAttribArray(location);

        // "Desligamos" o VBO, evitando assim que operações posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Agora repetimos todos os passos acima para atribuir um novo atributo a
        // cada vértice: uma cor (veja slide 113 do documento "Aula_04_Modelagem_Geometrica_3D.pdf").
        // Tal cor é definida como coeficientes RGBA: Red, Green, Blue, Alpha;
        // isto é: Vermelho, Verde, Azul, Alpha (valor de transparência).
        // Conversaremos sobre sistemas de cores nas aulas de Modelos de Iluminação.
        GLfloat color_coefficients[] = {
                // Cores dos vértices do cubo
                //  R     G     B     A
                1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 0
                1.0f, 0.5f, 0.0f, 1.0f, // cor do vértice 1
                0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 2
                0.0f, 0.5f, 1.0f, 1.0f, // cor do vértice 3
        };
        GLuint VBO_color_coefficients_id;
        glGenBuffers(1, &VBO_color_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_color_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_coefficients), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(color_coefficients), color_coefficients);
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Vamos então definir polígonos utilizando os vértices do array
        // model_coefficients.
        //
        // Para referência sobre os modos de renderização, veja slide 160 do
        // documento "Aula_04_Modelagem_Geometrica_3D.pdf".
        //
        // Este vetor "indices" define a TOPOLOGIA (veja slide 112 do documento
        // "Aula_04_Modelagem_Geometrica_3D.pdf").
        //
        GLuint indices[] = {
                // Definimos os índices dos vértices que definem as FACES de um cubo
                // através de 12 triângulos que serão desenhados com o modo de renderização
                // GL_TRIANGLES.
                0, 2, 3, // triângulo 1
                2, 3, 0
        };

        // Criamos um primeiro objeto virtual (SceneObject) que se refere às faces
        // coloridas do cubo.
        SceneObject plane;
        plane.name           = "wall";
        plane.first_index    = (void*)0; // Primeiro índice está em indices[0]
        plane.num_indices    = 6;       // Último índice está em indices[35]; total de 36 índices.
        plane.rendering_mode = GL_TRIANGLES; // Índices correspondem ao tipo de rasterização GL_TRIANGLES.

        // Adicionamos o objeto criado acima na nossa cena virtual (g_VirtualScene).
        g_VirtualScene["wall"] = plane;

        // Criamos um buffer OpenGL para armazenar os índices acima
        GLuint indices_id;
        glGenBuffers(1, &indices_id);

        // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);

        // Alocamos memória para o buffer.
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);

        // Copiamos os valores do array indices[] para dentro do buffer.
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);

        // NÃO faça a chamada abaixo! Diferente de um VBO (GL_ARRAY_BUFFER), um
        // array de índices (GL_ELEMENT_ARRAY_BUFFER) não pode ser "desligado",
        // caso contrário o VAO irá perder a informação sobre os índices.
        //
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // XXX Errado!
        //

        // "Desligamos" o VAO, evitando assim que operações posteriores venham a
        // alterar o mesmo. Isso evita bugs.
        glBindVertexArray(0);

        // Retornamos o ID do VAO. Isso é tudo que será necessário para renderizar
        // os triângulos definidos acima. Veja a chamada glDrawElements() em main().
        return vertex_array_object_id;
    }

};


#endif //FCG_TUNNEL_H
