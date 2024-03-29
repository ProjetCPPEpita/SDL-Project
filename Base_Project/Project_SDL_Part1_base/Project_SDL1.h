﻿// SDL_Test.h: Includedatei für Include-Standardsystemdateien
// oder projektspezifische Includedateien.

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <vector>
#include <stdlib.h>

// Defintions
constexpr double frame_rate = 60.0; // refresh rate
constexpr double frame_time = 1. / frame_rate;
constexpr unsigned frame_width = 1400; // Width of window in pixel
constexpr unsigned frame_height = 900; // Height of window in pixel
// Minimal distance of animals to the border
// of the screen
constexpr unsigned frame_boundary = 100;

constexpr char file_path_s[] = "../media/sheep.png";// Path to the sheep image
constexpr char file_path_sf[] = "../media/sheep.png";// Path to the sheep female image
constexpr char file_path_w[] = "../media/wolf.png"; // Path to the wolf image
constexpr char file_path_shepherd[] = "../media/shepherd.png"; // Path to the shepherd image

// Helper function to initialize SDL
void init();

class animal {
protected:
  SDL_Surface* window_surface_ptr_; // ptr to the surface on which we want the
                                    // animal to be drawn, also non-owning
  SDL_Surface* image_ptr_; // The texture of the sheep (the loaded image), use
                           // load_surface_for
  SDL_Rect position; // To have access to the position of each animal


  SDL_Rect point; // Animals will follow a virtual point to all move
    // in a different way
  char type; // To know the type of animal (sheep or wolf)
  int pv = 0;; // To distinguish sheeps and wolves
  float speed; // To move more or less quickly
  int sexe; //0 female 1 male


public:

    animal(const char *file_path, SDL_Surface* window_surface_ptr);

    ~animal();

  SDL_Rect get_position();
  SDL_Rect get_point();
  void set_point(SDL_Rect point);
  char get_type();
  int get_pv();
  int get_speed();
  int get_sexe();
  std::vector<animal*> death(SDL_Rect point, std::vector<animal*> storage);
  void draw();



    virtual void move(std::vector<animal*> *storage) = 0; // Animals move around, but in a different
                           // fashion depending on which type of animal
};

// Insert here:
// class sheep, derived from animal
class sheep : public animal {

private:

    Uint32 time_to_reproduce;
    Uint32 time_speed;

public:
    sheep(SDL_Surface* window_surface_ptr, char type);
    ~sheep(){}
    std::vector<animal*> reproduce(std::vector<animal*> storage);
    void fuite(std::vector<animal*> storage);
    void move(std::vector<animal *> *storage);



};

// Insert here:
// class wolf, derived from animal
// Use only sheep at first. Once the application works
// for sheep you can add the wolves
class wolf : public animal {
public:
    Uint32 time_limit= 9000;
    Uint32 timetolive;
    bool hasCaughtSheep;
    wolf(SDL_Surface* window_surface_ptr, char type, std::vector<animal*> storage);
    ~wolf(){}
    SDL_Rect get_target( std::vector<animal*> storage);
    void wolf_hunt();
    void move(std::vector<animal*> *storage);
};

// The "ground" on which all the animals live (like the std::vector
// in the zoo example).

class shepherd {
private:
    SDL_Surface* window_surface_ptr_; // ptr to the surface on which we want the
    // shepherd to be drawn, also non-owning
    SDL_Surface* image_ptr_; // The texture of the shepherd (the loaded image), use
    // load_surface_for

public:
    SDL_Rect shepherd_position; // To have access to the position of the shepherd
    SDL_Rect shepherd_get_position();
    void set_x_position(int x);
    void set_y_position(int y);
    void shepherd_draw();

    shepherd(const char *file_path, SDL_Surface* window_surface_ptr);
    void shepherd_move(SDL_Event event);
    ~shepherd(){}

};

class ground {
private:
  // Attention, NON-OWNING ptr, again to the screen
  SDL_Surface* window_surface_ptr_;

  // Some attribute to store all the wolves and sheep
  std::vector<animal*> storage;
    shepherd *shepherd_;

public:
    ground(SDL_Surface* window_surface_ptr); // Ctor
    ~ground(); // Dtor, again for clean up (if necessary)

    void add_animal(char type); // Add an animal

    void update(SDL_Surface *s , SDL_Event window_event_); // "refresh the screen": Move animals and draw them
    std::vector<animal*> get_storage();
};

// The application class, which is in charge of generating the window
class application {
private:
  // The following are OWNING ptrs
  SDL_Window* window_ptr_;
  SDL_Surface* window_surface_ptr_;
  SDL_Event window_event_;

  unsigned nb_sheep;
  unsigned nb_wolf;
  bool continuer = true;
  ground *ground_;
  // Other attributes here, for example an instance of ground

public:
  application(unsigned n_sheep, unsigned n_wolf); // Ctor
  ~application(); // dtor
  int loop(unsigned period); // main loop of the application.
                             // this ensures that the screen is updated
                             // at the correct rate.
                             // See SDL_GetTicks() and SDL_Delay() to enforce a
                             // duration the application should terminate after
                             // 'period' seconds
};
