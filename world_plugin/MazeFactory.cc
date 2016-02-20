#include "MazeFactory.hh"

#include <ignition/math/Pose3.hh>

namespace gazebo
{

const int MazeFactory::MAZE_SIZE = 16;
const float MazeFactory::UNIT = 0.18; //distance between centers of squares
const float MazeFactory::WALL_HEIGHT = 0.05;
const float MazeFactory::WALL_LENGTH = 0.16;
const float MazeFactory::WALL_THICKNESS = 0.012;
const float MazeFactory::BASE_HEIGHT= 0.1;
const float MazeFactory::PAINT_THICKNESS = 0.01;


MazeFactory::MazeFactory(): modelSDF() {}

void MazeFactory::Load(physics::WorldPtr _parent, sdf::ElementPtr _sdf)
{
  this->parent = _parent;
  node = transport::NodePtr(new transport::Node());
  node->Init(parent->GetName());
  sub = node->Subscribe("~/maze/regenerate", &MazeFactory::Regenerate, this);
}

void MazeFactory::Regenerate(ConstGzStringPtr &msg)
{
  std::string maze_filename = msg->data();

  sdf::ElementPtr model = LoadModel();
  sdf::ElementPtr base_link = model->GetElement("link");

  InsertWall(base_link, 0, 0, Direction::S);

  if (maze_filename == "random")
  {
    //create random maze here
  }
  else
  {
    //load maze from file
  }

  model->GetAttribute("name")->Set("my_maze");
  model->GetElement("pose")->Set(
    math::Pose(math::Vector3(0, 0, 0), math::Quaternion(0, 0, 0)));
  parent->InsertModelSDF(*modelSDF);
}

std::list<sdf::ElementPtr> MazeFactory::CreateWallVisual(float row, float col, Direction dir)
{
  float zero_offset = -UNIT * (MAZE_SIZE/2 - 1);
  float link_x = zero_offset + col * UNIT;
  float link_y = zero_offset + row * UNIT;

  msgs::Pose *visual_pose = CreatePose(0, 0, BASE_HEIGHT + (WALL_HEIGHT - PAINT_THICKNESS)/2,
                                       0, 0, 0, 0);
  msgs::Pose *paint_visual_pose = CreatePose(0, 0, BASE_HEIGHT + WALL_HEIGHT - PAINT_THICKNESS/2,
                                  0, 0, 0, 0);

  msgs::Geometry *visual_geo = CreateBoxGeometry(WALL_LENGTH,
                               WALL_THICKNESS,
                               WALL_HEIGHT - PAINT_THICKNESS);

  msgs::Geometry *paint_visual_geo = CreateBoxGeometry(WALL_LENGTH,
                                     WALL_THICKNESS,
                                     PAINT_THICKNESS);

  msgs::Visual visual;
  visual.set_name("v1");
  visual.set_allocated_geometry(visual_geo);
  visual.set_allocated_pose(visual_pose);

  msgs::Material_Script *paint_script = new msgs::Material_Script();
  std::string *uri = paint_script->add_uri();
  *uri = "file://media/materials/scripts/gazebo.material";
  paint_script->set_name("Gazebo/Red");

  msgs::Material *paint_material = new msgs::Material();
  paint_material->set_allocated_script(paint_script);

  msgs::Visual paint_visual;
  paint_visual.set_name("v1_paint");
  paint_visual.set_allocated_geometry(paint_visual_geo);
  paint_visual.set_allocated_pose(paint_visual_pose);
  paint_visual.set_allocated_material(paint_material);

  sdf::ElementPtr visualElem = msgs::VisualToSDF(visual);
  sdf::ElementPtr visualPaintElem = msgs::VisualToSDF(paint_visual);
  std::list<sdf::ElementPtr> visuals;
  visuals.push_front(visualElem);
  visuals.push_front(visualPaintElem);
  return visuals;
}

sdf::ElementPtr MazeFactory::CreateWallCollision(float row, float col, Direction dir)
{
  float zero_offset = -UNIT * (MAZE_SIZE/2 - 1);
  float link_x = zero_offset + col * UNIT;
  float link_y = zero_offset + row * UNIT;

  msgs::Pose *collision_pose = CreatePose(0, 0, BASE_HEIGHT + WALL_HEIGHT/2, 0, 0, 0, 0);

  msgs::Geometry *collision_geo = CreateBoxGeometry(WALL_LENGTH,
                                  WALL_THICKNESS,
                                  WALL_HEIGHT);

  msgs::Collision collision;
  collision.set_name("c1");
  collision.set_allocated_geometry(collision_geo);
  collision.set_allocated_pose(collision_pose);

  sdf::ElementPtr collisionElem = msgs::CollisionToSDF(collision);
  return collisionElem;
}

void MazeFactory::InsertWall(sdf::ElementPtr link, float row, float col, Direction dir)
{
  std::list<sdf::ElementPtr> walls_visuals = CreateWallVisual(0,0,Direction::S);
  sdf::ElementPtr walls_collision = CreateWallCollision(0,0,Direction::S);

  //insert all the visuals
  std::list<sdf::ElementPtr>::iterator list_iter = walls_visuals.begin();
  while (list_iter != walls_visuals.end())
  {
        link->InsertElement(*(list_iter++));
  }

  link->InsertElement(walls_collision);
}

msgs::Pose *MazeFactory::CreatePose(float px, float py, float pz,
                                    float ox, float oy, float oz, float ow)
{
  msgs::Vector3d *position = new msgs::Vector3d();
  position->set_x(px);
  position->set_y(py);
  position->set_z(pz);

  msgs::Quaternion *orientation = new msgs::Quaternion();
  orientation->set_x(ox);
  orientation->set_y(oy);
  orientation->set_z(oz);
  orientation->set_w(ow);

  msgs::Pose *pose = new msgs::Pose;
  pose->set_allocated_orientation(orientation);
  pose->set_allocated_position(position);

}

msgs::Geometry *MazeFactory::CreateBoxGeometry(float x, float y, float z)
{
  msgs::Vector3d *size = new msgs::Vector3d();
  size->set_x(x);
  size->set_y(y);
  size->set_z(z);

  msgs::BoxGeom *box = new msgs::BoxGeom();
  box->set_allocated_size(size);

  msgs::Geometry *geo = new msgs::Geometry();
  geo->set_type(msgs::Geometry_Type_BOX);
  geo->set_allocated_box(box);

  return geo;
}

sdf::ElementPtr MazeFactory::LoadModel()
{
  modelSDF.reset(new sdf::SDF);

  sdf::initFile("root.sdf", modelSDF);
  sdf::readFile("maze_base/model.sdf", modelSDF);

  return modelSDF->Root()->GetElement("model");
}

GZ_REGISTER_WORLD_PLUGIN(MazeFactory)
}
