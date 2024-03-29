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
#include <iostream>
#include <experimental/random>
#include <chrono>

// Returns true if x is in range [low..high], else false
bool inRange(int low, int high, int x)
{
    return ((x-high)*(x-low) <= 0);
}

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

void animal::set_point(SDL_Rect point)
{
    this->point = point;
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
int animal::get_sexe(){
    return this->sexe;
}
std::vector<animal*> ground::get_storage(){
    return this->storage;
}

SDL_Rect get_random(SDL_Rect point){

    // The point followed by each animal is chosen randomly
    int range_x = (frame_width-frame_boundary) - frame_boundary + 1;
    int range_y = (frame_height-frame_boundary) - frame_boundary + 1;
    point.x = rand() % range_x + frame_boundary;
    point.y = rand() % range_y + frame_boundary;

    return point;

}

std::vector<animal*> animal::death(SDL_Rect point, std::vector<animal*> storage) {
    for (int i=0; i< storage.size(); i++) {
        if (storage[i]->type == 'w')
            continue;
        if (inRange(storage[i]->position.x, storage[i]->position.x + 50, point.x) && inRange(storage[i]->position.y, storage[i]->position.y - 65, point.y)) {
            storage[i]->pv = 0;
        }
    }
    return storage;
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

sheep::sheep(SDL_Surface* window_surface_ptr, char type) : animal((sexe == 1) ? file_path_s : file_path_sf, window_surface_ptr){
    this->type = type;
    this->point = get_random(point);
    this->speed = 1;
    this->sexe = std::experimental::randint(0,1);
    this->time_to_reproduce = 0;
}

std::vector<animal*> sheep::reproduce(std::vector<animal *> storage) {

    for (auto target : storage){
        if (inRange(position.x, position.x + 50, target->get_position().x) && inRange(position.y, position.y - 60, target->get_position().y) ) {// si 2 moutons se rencontrent
            clock_t diff = (clock() - this->time_to_reproduce);
            if (target->get_type() == 's' && (this->sexe != target->get_sexe()) && diff > 10 * CLOCKS_PER_SEC) {
                storage.push_back(new sheep(window_surface_ptr_, 's'));
                this->time_to_reproduce = clock();

            }//même type et sont de sexe opposé

        }

    }
    return storage;
}

void sheep::fuite(std::vector<animal*> storage)
{
    for (auto target : storage)
    {
        if (target->get_type() == 's' || target->get_pv() !=  1)
            continue;
        double first_x = target->get_position().x - position.x;
        double  first_y = target->get_position().y - position.y;
        double dist = sqrt(first_x*first_x + first_y*first_y);

        if (dist < 200)
        {
            point.x = 2 * position.x - target->get_position().x ;
            point.y = 2 * position.y - target->get_position().y ;

            if (point.x < frame_boundary)
                point.x = frame_boundary + 10;
            if (point.x > frame_width - frame_boundary)
                point.x = frame_width - frame_boundary - 10;
            if (point.y < frame_boundary)
                point.y = frame_boundary + 10;
            if (point.y > frame_height - frame_boundary)
                point.y = frame_height - frame_boundary - 10;

            speed = 2;
            time_speed = clock();
        }
    }
}

void sheep::move(std::vector<animal *> *storage) {

    if ( clock() - time_speed > 1 * CLOCKS_PER_SEC)// fin du boost speed après 2 sec
        speed = 1;

    fuite(*storage);

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
    if (this->sexe == 0) //female
        *storage = reproduce(*storage);

    // If the animal touch the point, modify point position
    if (inRange(point.x - 2, point.x + 2, position.x) && inRange(point.y - 2, point.y + 2, position.y )) {
        this->point = get_random(this->point);
    }

}

wolf::wolf(SDL_Surface* window_surface_ptr, char type, std::vector<animal*> storage) : animal(file_path_w, window_surface_ptr){
    this->type = type;
    this->speed = 2;
    this-> timetolive = SDL_GetTicks();
    this-> hasCaughtSheep = false;

    /*animal *target = get_target(this->position, storage);
    if (target->get_type() == 'w') {
      this->point = get_random(this->point);
    }
    else {
      this->point = target->get_position();
    }
      std::cout<<"wpoint x " <<this->point.x <<'\n' <<"wpoint y " <<this->point.y<<'\n';*/

}

SDL_Rect wolf::get_target( std::vector<animal*> storage) {
    // on prend des moutons qui sont encore en vie
    double first_x;
    double first_y;
    int i = 0;
    while (storage[i]->get_pv() != 1 || storage[i]->get_type() == 'w')
    {
        i++;
        if (i >= storage.size() )
            return storage[0]->get_position();
    }
    first_x = storage[i]->get_position().x - position.x;
    first_y = storage[i]->get_position().y - position.y;
    double min = sqrt(first_x*first_x + first_y*first_y);
    SDL_Rect nearest = storage[i]->get_position();
    for (auto target : storage) {
        if (target->get_type() == 'w' || target->get_pv() == 0)
            continue;

        double diff_x = target->get_position().x - position.x;
        double diff_y = target->get_position().y - position.y;
        double dist = sqrt(diff_x*diff_x + diff_y*diff_y);
        if (dist < min) {
            min = dist;
            nearest = target->get_position();
        }

    }
    return nearest;
}
void wolf::wolf_hunt() {

    if  (SDL_GetTicks() - timetolive  < time_limit ) {
        if (this->hasCaughtSheep) {
            timetolive = SDL_GetTicks();
            this->hasCaughtSheep = false;
        }
    }
    else if (!(this->hasCaughtSheep)) {
        this->pv = 0;
        std::cout << "Wolf died of hunger and desire." << std::endl;
    }
}

void wolf::move(std::vector<animal*> *storage) {


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
    this->point = get_target(*storage);// Compare animal position and point followed position

    // If the wolf touch a sheep, the sheep die and the wolf has a new target
    if (inRange(this->point.x, this->point.x + 50, this->position.x + 30) && inRange(this->point.y, this->point.y - 65, this->position.y - 21))
    {
        *storage = death(this->point, *storage);
        this->hasCaughtSheep =true;
        this->point = get_target(*storage);
    }
    wolf_hunt();
    this->hasCaughtSheep = false;
}

SDL_Rect shepherd::shepherd_get_position() {
    return this->shepherd_position;
}

void shepherd::set_x_position(int x)
{
    this->shepherd_position.x = x;
}

void shepherd::set_y_position(int y)
{
    this->shepherd_position.y = y;
}

shepherd::shepherd(const char *file_path, SDL_Surface* window_surface_ptr){
    window_surface_ptr_ = window_surface_ptr;
    image_ptr_ = IMG_Load(file_path);
    std::cout << file_path<<'\n';
    if (!image_ptr_)
        std::cout << "image cannot be load";
    else
        std::cout << "image loaded";

    // Initialize animal attribute
    this->shepherd_position.x = frame_width/2;
    this->shepherd_position.y = frame_height/2;
}

void shepherd::shepherd_draw(){
    SDL_BlitSurface(image_ptr_, NULL, window_surface_ptr_, &shepherd_position);
}

void shepherd::shepherd_move(SDL_Event event) {

    if(event.type == SDL_KEYDOWN)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_z:
                this->shepherd_position.y = shepherd_get_position().y - 1;
                break;
            case SDLK_s:
                this->shepherd_position.y = shepherd_get_position().y + 1;
                break;
            case SDLK_q:
                this->shepherd_position.x = shepherd_get_position().x - 1;
                break;
            case SDLK_d:
                this->shepherd_position.x = shepherd_get_position().x + 1;
                break;
        }
    }
}

ground::ground(SDL_Surface* window_surface_ptr) {
    this->window_surface_ptr_ = window_surface_ptr;
    shepherd_ = new shepherd(file_path_shepherd, window_surface_ptr_);
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

void ground::update(SDL_Surface *s, SDL_Event window_event_ ){
    SDL_BlitSurface(s, NULL, window_surface_ptr_, nullptr);
    shepherd_->shepherd_move(window_event_);
    shepherd_->shepherd_draw();
    for (auto animal_ : storage) {
        if (animal_->get_pv() == 1) {
            animal_->move(&storage);
            animal_->draw();
            SDL_Delay(1);
            continue;
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

    ground_->add_animal('d');

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
float countsheep(std::vector<animal*> storage, bool vivant)
{
    int i = (vivant)? 1 : 0;
    float count = 0;
    for (auto target : storage)
    {
        if (target->get_type() == 's' && target->get_pv() == i)
            count++;
    }

    return count;
}
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

            ground_->update(s , window_event_);
            SDL_UpdateWindowSurface(window_ptr_);
            if (countsheep(ground_->get_storage(), false) >= nb_sheep)
            {
                break;
            }

            SDL_PollEvent(&window_event_);
            switch(window_event_.type)
            {
                case SDL_QUIT:
                    continuer = false;
                    break;
                case SDL_KEYDOWN:
                    ground_->update(s, window_event_);
                    break;
            }
        }
        float average = countsheep(ground_->get_storage(), true) / nb_sheep;
        std::cout << "End of the Game! Average sheep count: " << average << std::endl;

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