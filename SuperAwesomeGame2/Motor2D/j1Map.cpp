#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Map.h"
#include <math.h>
#include "j1Collision.h"
#include "j1Window.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	p2List_item <ImageLayers*>* item_imgLayer = nullptr;

	
	

	for (item_imgLayer = data.image_layers.start; item_imgLayer; item_imgLayer = item_imgLayer->next){

		//All image_width are the same, it does not matter which one to use
		

		SDL_Rect rect = { 0, 0, item_imgLayer->data->image_width, item_imgLayer->data->image_height };
	
		App->render->Blit(item_imgLayer->data->texture, item_imgLayer->data->position_x, item_imgLayer->data->position_y, &rect, SDL_FLIP_NONE);
		App->render->Blit(item_imgLayer->data->texture, item_imgLayer->data->position_x + data.image_layers.start->data->image_width -3, item_imgLayer->data->position_y, &rect, SDL_FLIP_NONE);

		if (item_imgLayer->data->position_x + data.image_layers.start->data->image_width < -App->render->camera.x *2)
		{
			item_imgLayer->data->position_x += item_imgLayer->data->image_width;
		}
		if ((-App->render->camera.x + App->render->camera.w )* 2 < item_imgLayer->data->position_x + data.image_layers.start->data->image_width)
		{
			item_imgLayer->data->position_x -= item_imgLayer->data->image_width;
		}
		

		if (item_imgLayer->data->name == "Capa de Imagen 1")
		{
			speedLayer = 1;


			

		}
		else if (item_imgLayer->data->name == "Capa de Imagen 2")
		{
			speedLayer = 3;
			


		}
		else if (item_imgLayer->data->name == "Capa de Imagen 3")
		{
			speedLayer = 5;
			


		}

		if (App->player->velocityX > 0)
		{

			item_imgLayer->data->position_x -= speedLayer;

		}
		else if (App->player->velocityX < 0)
		{

			item_imgLayer->data->position_x += speedLayer;
		}
		
	}

	// TODO 5(old): Prepare the loop to draw all tilesets + Blit
	/*p2List_item<MapLayer*>* layer = data.layers.start;
	p2List_item<TileSet*>* tileset = nullptr;
	iPoint coordenadas;
	//start from the end because the tileset order are inverted
	for (tileset = data.tilesets.end; tileset; tileset = tileset->prev) {
		while (layer != NULL)
		{
			for (uint i = 0; i < layer->data->width; i++) {
				for (uint j = 0; j < layer->data->height; j++) {
					uint id = layer->data->data[layer->data->Get(i, j)];
					SDL_Rect rect = tileset->data->GetTileRect(id);
					coordenadas = MapToWorld(i, j);
					App->render->Blit(tileset->data->texture, coordenadas.x, coordenadas.y, &rect);
				}
			}
			layer = layer->next;
		}
	}*/

	p2List_item<TileSet*>* item_tileset = nullptr;
	p2List_item<MapLayer*>* item_layer = nullptr;
	uint id = 0;
	for (item_tileset = data.tilesets.end; item_tileset; item_tileset = item_tileset->prev) {
		for (item_layer = data.layers.start; item_layer; item_layer = item_layer->next) {
			for (uint i = 0; i < item_layer->data->height; i++) {
				for (uint j = 0; j < item_layer->data->width; j++) {
					id = item_layer->data->data[item_layer->data->Get(j, i)];
					if (id != 0)
					{
						App->render->Blit(item_tileset->data->texture, MapToWorld(j, i).x, MapToWorld(j, i).y, &item_tileset->data->GetTileRect(id), SDL_FLIP_NONE);
						/*if (item_tileset->data->name == "TilesetWinterObjects128x128") {
							App->render->Blit(item_tileset->data->texture, MapToWorld(j, i).x, MapToWorld(j, i).y + 10, &item_tileset->data->GetTileRect(id));
						}*/
					}
				}
			}
		}
	}


	// TODO 10(old): Complete the draw function
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret(0,0);
	// TODO 8(old): Create a method that translates x,y coordinates from map positions to world positions
	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	// TODO 1: Add isometric map to world coordinates
	if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * data.tile_width*0.5f;
		ret.y = (x + y) * data.tile_height*0.5f;
	}
	
	return ret;
}


iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);
	// TODO 2: Add orthographic world to map coordinates

	// TODO 3: Add the case for isometric maps to WorldToMap
	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect rect = {0, 0, 0, 0};
	int relative_id = id - firstgid;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;

}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while(item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	p2List_item<MapObjects*>* item3;
	item3 = data.objects.start;

	while (item3 != NULL)
	{
		RELEASE(item3->data);
		item3 = item3->next;
	}
	data.objects.clear();

	p2List_item<ImageLayers*>* item4;
	item4 = data.image_layers.start;
	

	while (item4 != NULL)
	{
		RELEASE(item4->data);
		item4 = item4->next;
	}
	data.image_layers.clear();

	// Clean up the pugui tree
	map_file.reset();
	
	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
		bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.add(lay);
	}

	// Load Objects info ----------------------------------------------
	pugi::xml_node objects;
	for (objects = map_file.child("map").child("objectgroup").child("object"); objects && ret; objects = objects.next_sibling("object"))
	{

		MapObjects* obj = new MapObjects();

		ret = LoadObjects(objects, obj);

		if (ret == true)
			data.objects.add(obj);
	}

	// Load Background Image Layers info ----------------------------------------------
	pugi::xml_node imgLayers;
	for (imgLayers = map_file.child("map").child("imagelayer"); imgLayers && ret; imgLayers = imgLayers.next_sibling("imagelayer"))
	{

		ImageLayers* imgLayer = new ImageLayers();

		ret = LoadImageLayers(imgLayers, imgLayer);

		if (ret == true)
			data.image_layers.add(imgLayer);
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.create(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadImageLayers(pugi::xml_node& imagelayer_node, ImageLayers* set)
{
	bool ret = true;
	set->name = imagelayer_node.attribute("name").as_string();
	pugi::xml_node image = imagelayer_node.child("image");
	//set->position_x = imagelayer_node.attribute("offsetx").as_int();//*App->win->GetScale();
	//set->position_y = imagelayer_node.attribute("offsety").as_int();//*App->win->GetScale();
	set->position_x = image.next_sibling("properties").child("property").attribute("value").as_int();
	set->position_y = image.next_sibling("properties").child("property").next_sibling("property").attribute("value").as_int();
	set->image_width = image.attribute("width").as_int();
	set->image_height = image.attribute("height").as_int();
	set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
	
	//pugi::xml_node image = imagelayer_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing imagelayer xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->image_width = image.attribute("width").as_int();

		if (set->image_width <= 0)
		{
			set->image_width = w;
		}

		set->image_height = image.attribute("height").as_int();

		if (set->image_height <= 0)
		{
			set->image_height = h;
		}

		/*set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;*/
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();

	

	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;

}

bool j1Map::LoadObjects(pugi::xml_node& node, MapObjects* object)
{
	pugi::xml_node node_property = node.child("properties").child("property");
	object->id = node.attribute("id").as_int();
	object->name = "Colliders";
	if (node.attribute("name"))
	{
		object->name = node.attribute("name").as_string();
		object->StartPoint = { node.attribute("x").as_int(), node.attribute("y").as_int() };
		object->RectCollider = { 0,0,0,0 };
	}
	else
	{
		object->RectCollider = { node.attribute("x").as_int(),
								 node.attribute("y").as_int(),
							 	 node.attribute("width").as_int(),
								 node.attribute("height").as_int() };
	
		object->Collider_type = DefineType(node_property.attribute("value").as_int());
	}

	return true;
}

COLLIDER_TYPE j1Map::DefineType(int type_as_int)
{
	switch (type_as_int)
	{
	case 0:
		return COLLIDER_FLOOR;
	case 1:
		return COLLIDER_PLATAFORM;
	case 2:
		return COLLIDER_WATER;
	case 3:
		return COLLIDER_START_WALL;
	case 4:
		return COLLIDER_FINISH_LEVEL;
	case 5:
		return COLLIDER_PLAYER;
	default:
		return COLLIDER_NONE;
	}
}

