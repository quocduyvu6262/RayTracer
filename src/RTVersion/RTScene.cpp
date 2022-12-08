/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "../../include/RTVersion/RTScene.h"
#include "../../include/RTVersion/RTCube.h"
#include "../../include/RTVersion/RTObj.h"

// The scene init definition 
#include "RTScene.inl"


using namespace glm;
void RTScene::buildTriangleSoup(void){
   
    // Define stacks for depth-first search (DFS)
    std::stack < RTNode* > dfs_stack;
    std::stack < mat4 >  matrix_stack; // HW3: You will update this matrix_stack during the depth-first search while loop.
    
    // Initialize the current state variable for DFS
    RTNode* cur = node["world"]; // root of the tree
    mat4 cur_VM = camera -> view; // HW3: You will update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.
    
    // Update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
    dfs_stack.push(cur);
    matrix_stack.push(cur_VM);

    // Compute total number of connectivities in the graph; this would be an upper bound for
    // the stack size in the depth first search over the directed acyclic graph
    int total_number_of_edges = 0; 
    for ( const auto &n : node ) total_number_of_edges += n.second->childnodes.size();
    
    // If you want to print some statistics of your scene graph
    // std::cout << "total numb of nodes = " << node.size() << std::endl;
    // std::cout << "total number of edges = " << total_number_of_edges << std::endl;
    
    while( ! dfs_stack.empty() ){
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the number of edges in the graph.
        if ( dfs_stack.size() > total_number_of_edges ){
            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
            exit(-1);
        }
        
        // top-pop the stacks
        cur = dfs_stack.top();  dfs_stack.pop();
        /**
         * TODO: (HW3 hint: you should do something here)
         */
        cur_VM = matrix_stack.top(); matrix_stack.pop();
        // draw all the models at the current node
        for ( size_t i = 0; i < cur -> models.size(); i++ ){
            // Prepare to draw the geometry. Assign the modelview and the material.
            glm::mat4 curModelView = cur_VM * cur->modeltransforms[i];
            glm::mat3 inverseModelView = glm::inverse(glm::transpose(glm::mat3(curModelView)));
            Material* curMaterial = cur->models[i]->material;

            for(Triangle t : cur -> models[i] -> geometry -> elements){
                Triangle transformedTriangle;

                // add transformed position to triangle
                glm::vec4 transformedPos = curModelView * glm::vec4(t.P[0], 1);
                transformedTriangle.P.push_back(glm::vec3(transformedPos.x,transformedPos.y,transformedPos.z));
                transformedPos = curModelView * glm::vec4(t.P[1], 1);
                transformedTriangle.P.push_back(glm::vec3(transformedPos.x,transformedPos.y,transformedPos.z));
                transformedPos = curModelView * glm::vec4(t.P[2], 1);
                transformedTriangle.P.push_back(glm::vec3(transformedPos.x,transformedPos.y,transformedPos.z));

                // add transformed normal to triangle
                transformedTriangle.N.push_back(inverseModelView*t.N[0]);
                transformedTriangle.N.push_back(inverseModelView*t.N[1]);
                transformedTriangle.N.push_back(inverseModelView*t.N[2]);
                
                // add material
                transformedTriangle.material = curMaterial;

                // add transformed triangle to triangle soup
                triangle_soup.push_back(transformedTriangle);
            }
            
        }
        
        // Continue the DFS: put all the child nodes of the current node in the stack
        for ( size_t i = 0; i < cur -> childnodes.size(); i++ ){
            dfs_stack.push( cur -> childnodes[i] );
            /**
             * TODO: (HW3 hint: you should do something here)
             */
            matrix_stack.push( cur_VM * cur -> childtransforms[i] );
        }
        
    } // End of DFS while loop.    
}



