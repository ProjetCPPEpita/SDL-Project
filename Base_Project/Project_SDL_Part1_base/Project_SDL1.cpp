﻿// SDL_Test.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include "Project_SDL1.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <numeric>
#include <random>
#include <string>
#include <math.h>

void init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0)
    throw std::runtime_error("init():" + std::string(SDL_GetError()));

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags))
    throw std::runtime_error("init(): SDL_image could not initialize! "
                             "SDL_image Error: " +
                             std::string(IMG_GetError()));
}

SDL_Rect animal::get_position() {
  return this->position;
}

SDL_Rect animal::get_point(){
  return this->point;
}

char animal::get_type() {
  return this->type;
}

int animal::get_pv() {
  return this->pv;
}

int animal::get_speed() {
  return this->speed;
}

SDL_Rect get_random(SDL_Rect point){

  // The point followed by each animal is chosen randomly
  int range_x = (frame_width-frame_boundary) - frame_boundary + 1;
  int range_y = (frame_height-frame_boundary) - frame_boundary + 1;
  point.x = rand() % range_x + frame_boundary;
  point.y = rand() % range_y + frame_boundary;

  return point;

}

animal* get_target(SDL_Rect wolf, std::vector<animal*> storage) {
    // on prend des moutons qui sont encore en vie
    double first_x;
    double first_y;
    int i = 0;
    while (storage[i]->get_pv() != 1 && storage[i]->get_type() != 's')
    {
        i++;
    }
    first_x = storage[i]->get_position().x - wolf.x;
    first_y = storage[i]->get_position().y - wolf.y;
    double min = sqrt(first_x*first_x + first_y*first_y);
    animal *nearest = storage[i];
    for (auto target : storage) {
      if (target->get_type() == 's' && target->get_pv() == 1) {
        double diff_x = target->get_position().x - wolf.x;
        double diff_y = target->get_position().y - wolf.y;
        double dist = sqrt(diff_x*diff_x - diff_y*diff_y);
        if (dist < min)
          min = dist;
          nearest = target;
      }
    }

    return nearest;
}

void animal::death(SDL_Rect point, std::vector<animal*> storage) {
  for (auto target : storage) {
    if (target->get_position().x == point.x && target->get_position().y == point.y && target->get_type() == 's') {
      target->pv = 0;
    }
  }
}

animal::animal(const char *file_path, SDL_Surface* window_surface_ptr){
  window_surface_ptr_ = window_surface_ptr;
  image_ptr_ = IMG_Load(file_path);
  std::cout << file_path<<'\n';
  if (!image_ptr_)
      std::cout << "image cannot be load";
  else
      std::cout << "image loaded";

  // Initialize animal attribute
  this->position = get_random(position);
  this->pv = 1;
}

animal::~animal(){
    SDL_FreeSurface(image_ptr_);
    SDL_FreeSurface(window_surface_ptr_);
};

void animal::draw(){
  SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &position);
}

sheep::sheep(SDL_Surface* window_surface_ptr, char type) : animal(file_path_s, window_surface_ptr){
  this->type = type;
  this->point = get_random(point);
  std::cout << "point.x =" << this->point.x <<"\npoint.y="<< this->point.y<<'\n';
  this->speed = 1;
}

void sheep::move(std::vector<animal*> storage) {

  // Compare animal position and point followed position
  if (this->point.x > this->position.x){
      this->position.x += this->speed;
    if (this->point.y > this->position.y){
      this->position.y += this->speed;
    }
    else if (this->point.y < this->position.y){
      this->position.y -= this->speed;
    }
  }
  else if (this->point.x < this->position.x){
      this->position.x -= this->speed;
    if (this->point.y > this->position.y){
      this->position.y += this->speed;
    }
    else if (this->point.y < this->position.y){
      this->position.y -= this->speed;
    }
  }
  else if (this->point.x == this->position.x)
  {
      if (this->point.y > this->position.y){
          this->position.y += this->speed;
      }
      else if (this->point.y < this->position.y){
          this->position.y -= this->speed;
      }
  }



  // If the animal touch the point, modify point position
  if (this->position.x == this->point.x && this->position.y == this->point.y) {
    this->point = get_random(this->point);
  }
}

wolf::wolf(SDL_Surface* window_surface_ptr, char type, std::vector<animal*> storage) : animal(file_path_w, window_surface_ptr){
  this->type = type;
  this->speed = 0.5;

  animal *target = get_target(this->position, storage);
  if (target->get_type() == 'w') {
    this->point = get_random(this->point);
  }
  else {
    this->point = target->get_position();
  }


}

void wolf::move(std::vector<animal*> storage) {

  // Compare animal position and point followed position
  if (this->point.x > this->position.x){
      this->position.x += this->speed;
    if (this->point.y > this->position.y){
      this->position.y += this->speed;
    }
    else if (this->point.y < this->position.y){
      this->position.y -= this->speed;
    }
  }
  else if (this->point.x < this->position.x){
      this->position.x -= this->speed;
    if (this->point.y > this->position.y){
      this->position.y += this->speed;
    }
    else if (this->point.y < this->position.y){
      this->position.y -= this->speed;
    }
  }
  else if (this->point.x == this->position.x)
  {
      if (this->point.y > this->position.y){
          this->position.y += this->speed;
      }
      else if (this->point.y < this->position.y){
          this->position.y -= this->speed;
      }
  }
    this->point = get_target(this->position, storage)->get_position();

  // If the wolf touch a sheep, the sheep die and the wolf has a new target
  if (this->position.x == this->point.x && this->position.y == this->point.y) // faire une hitbox psk
  {
    death(this->point, storage);
    this->point = get_target(this->position, storage)->get_position();
  }
}

ground::ground(SDL_Surface* window_surface_ptr) {
  this->window_surface_ptr_ = window_surface_ptr;
}

ground::~ground(){
  // Free SDL Surface
  if (window_surface_ptr_)
      SDL_FreeSurface(window_surface_ptr_);
  window_surface_ptr_ = nullptr;

  // Clear storage shared_ptr
  for (auto target : storage) {
    target->~animal();
  }
  storage.clear();

  // Quit the Surface
  SDL_Quit();

} // Dtor, again for clean up (if necessary)

void ground::add_animal(char type){
  // If it's a sheep
  if (type == 's') {
    this->storage.push_back(new sheep(window_surface_ptr_, 's'));
  }
  else {
    this->storage.push_back(new wolf(window_surface_ptr_, 'w', storage));
  }
} // Add an animal

void ground::update(SDL_Surface *s){
  SDL_BlitSurface(s, NULL, window_surface_ptr_, nullptr);
  for (auto animal_ : storage) {
    if (animal_->get_pv() == 1) {
      animal_->move(storage);
      animal_->draw();
    }
  }
} // "refresh the screen": Move animals and draw them
  // Possibly other methods, depends on your implementation

application::application(unsigned n_sheep, unsigned n_wolf){
    window_ptr_ = SDL_CreateWindow("Sheep and Wolves",SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   frame_width,
                                   frame_height,
                                   SDL_WINDOW_SHOWN);
    window_surface_ptr_ = SDL_GetWindowSurface(window_ptr_);

    nb_sheep = n_sheep;
    nb_wolf = n_wolf;

    ground_ = new ground(window_surface_ptr_);

    for (unsigned n = nb_sheep; n > 0; n--)
      ground_->add_animal('s');
    for (unsigned n = nb_wolf; n > 0; n--)
      ground_->add_animal('w');

} // Ctor

application::~application(){
    if (window_ptr_)
        SDL_DestroyWindow(window_ptr_);
    if (window_surface_ptr_)
        SDL_FreeSurface(window_surface_ptr_);
    window_ptr_ = nullptr;
    window_surface_ptr_ = nullptr;
    SDL_Quit();
} // dtor

int application::loop(unsigned period){

    if( window_ptr_ )
    {
        SDL_Surface *s;
        /* Creating the surface. */
        s = SDL_CreateRGBSurface(0, frame_width, frame_height, 32, 0, 0, 0, 0);
        SDL_FillRect(s, NULL, SDL_MapRGB(s ->format, 0, 255, 0));
        SDL_BlitSurface(s, NULL, window_surface_ptr_, nullptr);
        SDL_UpdateWindowSurface(window_ptr_);
        srand(time(NULL));

        while((SDL_GetTicks() < (period*1000)) && continuer ) {

            ground_->update(s);
            SDL_UpdateWindowSurface(window_ptr_);

            SDL_PollEvent(&window_event_);
            switch(window_event_.type)
            {
                case SDL_QUIT:
                    continuer = false;
                    break;
            }
        }

    }
    else
    {
        fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
    }
    return 0;
} // main loop of the application.
  // this ensures that the screen is updated
  // at the correct rate.
  // See SDL_GetTicks() and SDL_Delay() to enforce a
  // duration the application should terminate after
  // 'period' seconds

namespace {
// Defining a namespace without a name -> Anonymous workspace
// Its purpose is to indicate to the compiler that everything
// inside of it is UNIQUELY used within this source file.
/*
SDL_Surface* load_surface_for(const std::string& path,
                              SDL_Surface* window_surface_ptr) {

  // Helper function to load a png for a specific surface
  // See SDL_ConvertSurface
}*/
} // namespace