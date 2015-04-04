#include "Commons.h"
#include "App.h"
#include "AppInterface.h"
#include "TransformationMatrices.h"
#include "OpenFileDialog.h"
using namespace boost::algorithm;

void AppInterface::initialize(GLFWwindow * window)
{
    ImGui_ImplGlfwGL3_Init(window, true);
}

void AppInterface::draw()
{
    // inmediate user interface new frame
    ImGui_ImplGlfwGL3_NewFrame();
    {
        static ImVec2 stackedSize;
        ImGuiIO& io = ImGui::GetIO();
        // performance window
        {
            ImGui::SetNextWindowSize(ImVec2(150, 50));
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 150 - 3,
                                           io.DisplaySize.y - 50 - 3));
            ImGui::Begin("Performance Window", nullptr,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
            ImGui::Text("Performance");
            ImGui::Separator();
            ImGui::Text("FPS: (%.1f)", io.Framerate);
            ImGui::End();
        }
        // mesh data input window
        {
            ImGui::SetNextWindowPos(ImVec2(3, 3));
            ImGui::Begin("Terrain Generation Options", nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::SliderInt("Mesh Resolution", &meshResolution, 2, 11,
                             std::to_string((int)std::pow(2, meshResolution) + 1).c_str());
            ImGui::InputInt("Heightmap Resolution", &heightmapResolution, 1, 16);
            // bound the int range
            heightmapResolution = std::max(16, std::min(8192, heightmapResolution));
            // generate new terrain on button press

            if(ImGui::Button("Generate Terrain"))
            {
                App::Instance()->getTerrain().createTerrain(heightmapResolution);
                App::Instance()->getTerrain().createMesh(meshResolution);
            }

            ImGui::SameLine();

            if(ImGui::Button("Save Heightmap"))
            {
                using namespace boost::posix_time;
                // set string format
                static std::locale loc(std::cout.getloc(), new time_facet("%Y-%m-%d_%H-%M-%S"));
                // parse it to a std::string
                std::basic_stringstream<char> wss;
                wss.imbue(loc);
                // get current time
                wss << second_clock::universal_time();
                // write terrain to file, concatenate timestamp
                App::Instance()->getTerrain().Heightmap().writeToFile("terrain" + wss.str());
            }

            ImGui::Separator();

            if(ImGui::SliderFloat("Maximum Height", &maxHeight, 0.001f, 15.0f))
            {
                TransformationMatrices::Model(
                    glm::scale(glm::mat4(), glm::vec3(1, maxHeight, 1))
                );
            }

            stackedSize = ImGui::GetWindowSize();
            ImGui::End();
        }
        // rendering options
        {
            ImGui::SetNextWindowPos(ImVec2(3, stackedSize.y + 6));
            ImGui::Begin("Rendering Options", nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Checkbox("Use Wireframe Mode", &wireframeMode);

            if(wireframeMode)
            {
                gl.PolygonMode(Face::FrontAndBack, PolygonMode::Line);
            }

            stackedSize.x += ImGui::GetWindowSize().x;
            stackedSize.y += ImGui::GetWindowSize().y;
            ImGui::End();
        }
        // terrain textures
        {
            ImGui::SetNextWindowPos(ImVec2(3, stackedSize.y + 9));
            ImGui::Begin("Terrain Textures", nullptr,
                         ImGuiWindowFlags_AlwaysAutoResize);
            static float ranges[8] = {0.0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0};
            ImGui::Text("Available Ranges");

            for(int i = 0; i < 4; i++)
            {
                if(i > 0) ImGui::SameLine();

                bool rangeChanged = false;
                ImGui::PushID(i);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(i / 7.0f, 0.5f, 0.5f));

                if(ImGui::VSliderFloat("##t", ImVec2(17, 120),
                                       &ranges[2 * i], 0.0f, 1.0f, ""))
                    rangeChanged ^= true;

                if(ImGui::IsItemActive() || ImGui::IsItemHovered())
                    ImGui::SetTooltip("%.3f", ranges[2 * i]);

                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImColor::HSV(i / 7.0f, 0.5f, 0.5f));

                if(ImGui::VSliderFloat("##v", ImVec2(17, 120),
                                       &ranges[2 * i + 1], 0.0f, 1.0f, ""))
                    rangeChanged = true;

                ImGui::PopStyleColor();

                if(ImGui::IsItemActive() || ImGui::IsItemHovered())
                    ImGui::SetTooltip("%.3f", ranges[2 * i + 1]);

                // set proper values ranges
                for(int j = 0; j < 8; j++)
                {
                    ranges[j] = clamp(
                                    ranges[j],
                                    j == 0 ? 0.0 : ranges[j - 1],
                                    j == 7 ?  1.0 : ranges[j + 1]
                                );
                }

                // set values
                if(rangeChanged)
                {
                    App::Instance()->getTerrain()
                    .TerrainTextures()
                    .SetTextureRange(i, ranges[i * 2], ranges[i * 2 + 1]);
                }

                ImGui::PopID();
            }

            for(int i = 0; i < 4; i++)
            {
                ImGui::PushID(i * 2 + 1);
                static ImTextureID texId[4] = {};

                if(ImGui::ImageButton(texId[i], ImVec2(34, 34)))
                {
                    static OpenFileDialog *fileDialog = new OpenFileDialog();
                    fileDialog->ShowDialog();
                    // load texture from filename
                    App::Instance()->getTerrain()
                    .TerrainTextures()
                    .loadTexture(fileDialog->FileName, i);
                    // assign texture to texid interfaces
                    texId[i] = (void *)(intptr_t)App::Instance()->getTerrain()
                               .TerrainTextures()
                               .UITextureId(i);
                }

                if(ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();
                    ImGui::Image(
                        texId[i],
                        ImVec2(200, 200),
                        ImVec2(0.0, 0.0), ImVec2(1.0, 1.0),
                        ImColor(255, 255, 255, 255),
                        ImColor(255, 255, 255, 128)
                    );
                    ImGui::EndTooltip();
                }

                i < 4 ? ImGui::SameLine() : 0;
                ImGui::PopID();
            }

            ImGui::End();
        }
    }
}

void AppInterface::render()
{
    if(wireframeMode)
    {
        gl.PolygonMode(Face::FrontAndBack, PolygonMode::Fill);
    }

    ImGui::Render();
}

void AppInterface::terminate()
{
    ImGui_ImplGlfwGL3_Shutdown();
}

AppInterface::AppInterface()
{
    this->maxHeight = 1.0;
    this->meshResolution = 6;
    this->heightmapResolution = 256;
    this->wireframeMode = false;
}


AppInterface::~AppInterface()
{
}
