import Qt3D.Core 2.0
import Qt3D.Render 2.0

Material {

    ShaderProgram {
        id: drawShader
        vertexShaderCode:   loadSource("qrc:/shaders/particles.vert")
        fragmentShaderCode: loadSource("qrc:/shaders/particles.frag")
    }

    effect: Effect {
        techniques: [
            Technique {
                renderPasses: [
                    RenderPass {
                        shaderProgram: drawShader
                        // We assume the mesh to be drawn will also receive
                        // Vertex buffers attributes that will be used to position and color
                    }
                ]
                filterKeys: [
                    FilterKey { name: "type"; value: "draw" }
                ]
                graphicsApiFilter {
                    api: GraphicsApiFilter.OpenGL
                    profile: GraphicsApiFilter.CoreProfile
                    majorVersion: 4
                    minorVersion: 3
                }
            }
        ] // techniques
    }
}

// vim: ts=3 sw=3 sts=0 noet ffs=unix ft=qml :
