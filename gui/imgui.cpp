// Dear ImGui: standalone example application for GLFW + OpenGL 3, using
// programmable pipeline (GLFW is a cross-platform general purpose library for
// handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation,
// etc.) If you are new to Dear ImGui, read documentation from the docs/ folder
// + read the top of imgui.cpp. Read online:
// https://github.com/ocornut/imgui/tree/master/docs

#include "include/data.hpp"
#include <glad/glad.h>

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <stdexcept>
#include <stdio.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to
// maximize ease of testing and compatibility with old VS compilers. To link
// with VS2010-era libraries, VS2015+ requires linking with
// legacy_stdio_definitions.lib, which we do using this pragma. Your own project
// should not be affected, as you are likely to link with a newer binary of GLFW
// that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) &&                                 \
  !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include <filesystem>
#include <iostream>
#include <variant>

#include <nfd.hpp>

#include <cgns-tools.hpp>

#include "include/frameBuffer.hpp"
#include "include/helpers.hpp"
#include "include/shader.hpp"

// using cgns_tools::gui::opengl_fn;

static void
glfw_error_callback(int error, const char* description)
{
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int
main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
    return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
  // GL ES 2.0 + GLSL 100
  const char* glsl_version = "#version 100";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
  // GL 3.2 + GLSL 150
  const char* glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
  // GL 3.0 + GLSL 130
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+
  // only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 3.0+ only
#endif

  // Create window with graphics context
  GLFWwindow* window = glfwCreateWindow(1280, 720, "cgnstools", NULL, NULL);
  if (window == NULL)
    return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |=
    ImGuiConfigFlags_NavEnableKeyboard;             // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable
  // Multi-Viewport / Platform Windows

  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls

  // Setup Dear ImGui style
  //   ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();

  // construct font path
  const std::filesystem::path file_path{ __FILE__ };
  const auto source_root_path = file_path.parent_path();
  const std::filesystem::path source_font_path{ "font/Inter-V.ttf" };
  const auto font_path = source_root_path / source_font_path;

  io.Fonts->AddFontFromFileTTF(font_path.c_str(), 16.0f);

  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

  // Our state
  //   bool show_demo_window = true;
  //   bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // initialize GLAD
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  cgns_tools::gui::data data{};

  cgns_tools::gui::shader shader{};
  // camera mCamera{ glm::vec3(0, 0, 3), 45.0f, 1.3f, 0.1f, 100.0f };

  cgns_tools::gui::frameBuffer frameBuffer{};

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application, or clear/overwrite your copy of the mouse data.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application, or clear/overwrite your copy of the
    // keyboard data. Generally you may always pass all inputs to dear imgui,
    // and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the docking environment
    // ImGuiWindowFlags windowFlags =
    //     ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
    //     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
    //     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
    //     ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiID dockspaceID = ImGui::GetID("DockSpace");
    if (!ImGui::DockBuilderGetNode(dockspaceID))
    {
      ImGui::DockBuilderRemoveNode(dockspaceID);
      ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None);

      ImGuiID dock_main_id = dockspaceID;
      ImGuiID dock_up_id = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Up, 0.05f, nullptr, &dock_main_id);
      ImGuiID dock_right_id = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Right, 0.2f, nullptr, &dock_main_id);
      ImGuiID dock_left_id = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Left, 0.2f, nullptr, &dock_main_id);
      ImGuiID dock_down_id = ImGui::DockBuilderSplitNode(
        dock_main_id, ImGuiDir_Down, 0.2f, nullptr, &dock_main_id);
      ImGuiID dock_down_right_id = ImGui::DockBuilderSplitNode(
        dock_down_id, ImGuiDir_Right, 0.6f, nullptr, &dock_down_id);

      // ImGui::DockBuilderDockWindow("Actions", dock_up_id);
      // ImGui::DockBuilderDockWindow("Hierarchy", dock_right_id);
      ImGui::DockBuilderDockWindow("Properties", dock_left_id);
      // ImGui::DockBuilderDockWindow("Console", dock_down_id);
      ImGui::DockBuilderDockWindow("Dear ImGui Demo", dock_down_right_id);
      ImGui::DockBuilderDockWindow("Viewer", dock_main_id);

      ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_up_id);
      node->LocalFlags |= (static_cast<int>(ImGuiDockNodeFlags_NoTabBar) |
                           static_cast<int>(ImGuiDockNodeFlags_NoResize));

      ImGui::DockBuilderFinish(dock_main_id);
    }
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

    ImGui::End();

    // if (ImGui::BeginMainMenuBar()) {
    //   if (ImGui::BeginMenu("File")) {
    //     // ShowExampleMenuFile();
    //     ImGui::EndMenu();
    //   }
    //   if (ImGui::BeginMenu("Edit")) {
    //     if (ImGui::MenuItem("Undo", "CTRL+Z")) {
    //     }
    //     if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
    //     } // Disabled item
    //     ImGui::Separator();
    //     if (ImGui::MenuItem("Cut", "CTRL+X")) {
    //     }
    //     if (ImGui::MenuItem("Copy", "CTRL+C")) {
    //     }
    //     if (ImGui::MenuItem("Paste", "CTRL+V")) {
    //     }
    //     ImGui::EndMenu();
    //   }
    //   ImGui::EndMainMenuBar();
    // }

    if (ImGui::Begin("Viewer"))
    {

      ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
      glm::vec2 mSize = { viewportPanelSize.x, viewportPanelSize.y };
      const auto width = viewportPanelSize.x;
      const auto height = viewportPanelSize.y;

      // mCamera.set_aspect(mSize.x / mSize.y);
      // mCamera.update(shader);

      // sometimes at startup a height of zero is encountered
      if (!frameBuffer.fits(width, height) && width > 0 && height > 0)
      {
        frameBuffer.resize(width, height);
      }

      frameBuffer.bind();

      if (data)
      {
        data.render(shader);
      }

      frameBuffer.unbind();

      // add rendered texture of frame buffer to current imgui window
      ImGui::Image(reinterpret_cast<void*>(frameBuffer.get_texture()),
                   ImVec2{ mSize.x, mSize.y },
                   ImVec2{ 0, 1 },
                   ImVec2{ 1, 0 });
    }
    ImGui::End();

    // ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Properties"))
    {
      if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
      {
        if (ImGui::Button("Open"))
        {
          NFD::Guard nfdGuard;
          NFD::UniquePath outPath;
          nfdfilteritem_t filterItem[1] = { { "CGNS", "cgns" } };

          nfdresult_t result = NFD::OpenDialog(outPath, filterItem, 1);

          if (result == NFD_OKAY)
          {
            std::cout << "Success!" << std::endl << outPath.get() << std::endl;
            data.loadFile(outPath.get());
          }
          else if (result == NFD_CANCEL)
          {
            std::cout << "User pressed cancel." << std::endl;
          }
          else
          {
            std::cout << "Error: " << NFD::GetError() << std::endl;
          }
        }
        ImGui::SameLine(0, 5.0f);
        ImGui::Text("%s", data.file().c_str());
      }

      if (data)
      {
        if (ImGui::TreeNodeEx("CGNS"))
        {

          for (const auto& base : data()->bases)
          {
            if (ImGui::TreeNodeEx(base.name.c_str()))
            {

              if (ImGui::TreeNodeEx("Zones"))
              {

                for (const auto& zone : base.zones)
                {
                  std::visit(
                    [](auto&& zone)
                    {
                      if (ImGui::TreeNodeEx(zone.name.c_str()))
                      {
                        ImGui::TreePop();
                      }
                    },
                    zone);
                }

                ImGui::TreePop();
              }

              if (ImGui::TreeNodeEx("Families"))
              {

                for (const auto& family : base.families)
                {
                  if (ImGui::TreeNodeEx(family.name.c_str()))
                  {
                    ImGui::TreePop();
                  }
                }

                ImGui::TreePop();
              }

              ImGui::TreePop();
            }
          }

          ImGui::TreePop();
        }
      }
    }
    ImGui::End();

    ImGui::ShowDemoWindow();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w,
                 clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w,
                 clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}