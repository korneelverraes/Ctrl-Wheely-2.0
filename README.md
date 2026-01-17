# Ctrl-Wheely
*Optimalisatie van autodashboard-interactie door middel van een fysieke verplaatsbare knop in de middenconsole en een head-up display.* 

*Projectteam: Korneel Verraes; Warre Robbe; Seppe Nijs; Bram Tops*

19/01/2026

## Titel 1

## Pygame
The project includes a Head-Up Display (HUD) that projects information onto the windshield, indicating which function is currently being controlled via the dial.

In a previous iteration of the project, the HUD was developed using ProtoPie and connected to an Arduino. Due to the limitations of ProtoPie, all scenes are now fully implemented in Python using the PyGame library.

The user interface that was designed and tested within the course User-Centered Design has been recreated almost one-to-one in PyGame. This approach ensures that the validated and positively received UX is preserved in the final implementation.

Within PyGame, it is possible to receive serial data directly from the Arduino, to which the dial is connected. This incoming serial input is used to dynamically update elements of the HUD in real time.

As a result, the system is not a simulated “Wizard of Oz” interface, but a fully functional and directly responsive HUD that accurately reflects the user’s interactions with the dial.

The PyGame implementation was further extended with additional functionalities, including: Extra buttons, Submenus, Variable values per passenger or driver and Additional configurable interface elements

The PyGame Code can be uploaded in Python. You only need to fill in the right COM-port in line ... and for use as HUD you need to remove the "#" in line ... and line ... to mirror the screen over the x-asis.
```py
#libraries
import pygame
import sys
import math
import serial


#Initialisation
pygame.init()

# Screen sizes
SCREEN_WIDTH = 1200
SCREEN_HEIGHT = 650
TITLE = "HUD Ctrl-Wheely"

# Colors
DONKER_GRIJS = (0, 0, 0) # Basic HUD color
WIT = (255, 255, 255)
WIT_LICHT = (150, 150, 150)
ROOD = (255, 0, 0)
BLAUW = (0, 150, 255)
MAGENTA = (200, 50, 150)
GROEN_LICHT = (100, 255, 100)
GRIJS_LICHT = (150, 150, 150)

# create screen
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.FULLSCREEN | pygame.SCALED)
pygame.display.set_caption(TITLE)

# Lettertypes
font_groot = pygame.font.SysFont('Arial', 80, bold=True)
font_medium = pygame.font.SysFont('Arial', 30)
font_klein = pygame.font.SysFont('Arial', 15)
font_kleinmed = pygame.font.SysFont('Arial', 22)

clock = pygame.time.Clock()

#Simulation data variables (eventually changeable at a later state with serial data)
DATA = {
    'snelheid': 112,
    'temperatuurR': 20.0,
    'temperatuurL': 20.0,
    'accu_percentage': 89,
    'bereik_km': 435,
    'max_snelheid': 120
}

active_status = ""
active_driver = ""
balk_statusL = 0
balk_statusR = 0

#Serial port Arduino
try:
    ser = serial.Serial('COM5', 115200, timeout=0)
except:
    ser = None
    print("Arduino niet gevonden")

#load images
try:
    nav = pygame.image.load('pos.nav.png').convert_alpha()
    nav = pygame.transform.scale(nav, (75, 75))
except pygame.error:
    nav = pygame.Surface((75, 75), pygame.SRCALPHA)
    nav.fill((0, 0, 255, 180))
    
try:
    phone = pygame.image.load('pos.phone.png').convert_alpha()
    phone = pygame.transform.scale(phone, (75, 75))
except pygame.error:
    phone = pygame.Surface((75, 75), pygame.SRCALPHA)
    phone.fill((255, 0, 255, 180))

try:
    tempr = pygame.image.load('pos.tempR.png').convert_alpha()
    tempr = pygame.transform.scale(tempr, (75, 75))
except pygame.error:
    tempr = pygame.Surface((75, 75), pygame.SRCALPHA)
    tempr.fill((255, 0, 255, 180))

try:
    templ = pygame.image.load('pos.tempL.png').convert_alpha()
    templ = pygame.transform.scale(templ, (75, 75))
except pygame.error:
    templ = pygame.Surface((75, 75), pygame.SRCALPHA)
    templ.fill((255, 0, 255, 180))

try:
    music = pygame.image.load('pos.music.png').convert_alpha()
    music = pygame.transform.scale(music, (75, 75))
except pygame.error:
    music = pygame.Surface((75, 75), pygame.SRCALPHA)
    music.fill((255, 0, 255, 180))
    
try:
    micro = pygame.image.load('micro.png').convert_alpha()
    micro = pygame.transform.scale(micro, (75, 75))
except pygame.error:
    micro = pygame.Surface((75, 75), pygame.SRCALPHA)
    micro.fill((0, 200, 0, 180))
try:
    stoel = pygame.image.load('carseat.png').convert_alpha()
    stoel = pygame.transform.scale(stoel, (50,50))
except pygame.error as e:
    print(f"Kon carseat.png niet laden: {e}")
    stoel = None

#Vierkant is used to draw a rectangle above a menu to draw a new menu over it by changing menus
vierkant = pygame.Rect(900, 130, 300, 800)

#Content lists for different menus
nav_favo = [
    "Home",
    "Work",
    "Nearest charging station",
    "Supermarket",
    "Gym",
    "Parents",
    "Cinema"
]

nav_recent = [
    "Hogebrugstraat 1, LO",
    "Boerderijstraat, Kortrijk",
    "Colruyt, Kortrijk",
    "Cinalco Medics, Brussel"]

nav_laadstations = [
    "Tesla Supercharger - 1 km",
    "Fastned Decathlon - 1,5 km",
    "Boost Charge - 5 km",
    "Fastned Jumbo - 9 km"]

nav_poi = [
    "Supermarkt",
    "Bakker",
    "Slager",
    "Doe-Het-Zelf",
    "Café's",
    "Ontbijt"]


phone_alle = [
    "Alain Vandam",
    "Bart",
    "Bucky",
    "Dirk",
    "Franky Loosvelt",
    "Guido Pallemans",
    "Kai",
    "Korrelneel",
    "Kwienten",
    "Liesje",
    "Michel Drets",
    "Sammy Tanghe",
    "Sippe",
    "Tessa",
    "Warre"
]

phone_favo = [
    "mama",
    "papa",
    "mokke",
    "Korrelneel",
    "Bart",
    "Sippe",
    "Warre"]

phone_recent = [
    "mokke",
    "Bucky",
    "Korrelneel",
    "Bart",
    "papa",
    "Sippe"]

music_spotify = [
    {"title": "Never Gonna Give You Up", "artist": "Rick Astley", "image": "album_a.jpg"},
    {"title": "Get Wrecked", "artist": "Dual Damage", "image": "album_b.jpg"},
    {"title": "Rhythm Nation", "artist": "Janet Jackson", "image": "album_c.jpg"},
    {"title": "Run To You", "artist": "Bryan Adams", "image": "album_d.jpg"},
    {"title": "Up Café", "artist": "Kartje Kilo", "image": "album_e.png"},
]

music_radio = [
    "Joe",
    "MNM",
    "NRJ",
    "Nostalgie",
    "Qmusic",
    "Radio 1",
    "Radio 2",
    "Studio Brussel (Stubru)",
    "Willy",
    "Zenith FM" 
]


    

phone_lijsten = {0: phone_alle,
                 1: phone_favo,
                 2: phone_recent}

nav_lijsten = {0: nav_favo,
               1: nav_recent,
               2: nav_laadstations,
               3: nav_poi}

music_lijsten = {0: music_spotify,
                 1: music_radio,
                 2: music_spotify,
                 3: music_spotify}



navi_keuze = ["Favorites",
              "Recente Bestemmingen",
              "Dichtsbijzijnde laadstations",
              "POI"]

phone_keuze = ["Alle Contacts",
               "Favorieten",
               "Recent"]

music_keuze = ["Spotify",
               "Radio",
               "Bluetooth",
               "USB"]

#Variables menu indexes, apart settings per person, ...
selected_index = 0
nav_index = 0
phone_index = 0
music_index = 0
scroll_offset = 0
current_song = 1

CENTER_SIZE = 150
SIDE_SIZE = 50
ALBUM_Y = 200

ventil = 1
ruit = 0

navig = 0

indexphone = 0

indexmusic = 0

iconmusic = 0
calling = 0
start_calling = 0
calling_timer = 0

dest = 0

micro_start_time = 0
microvalue = 0

actieve_gps = ""
actieve_phone = ""

#load music album images
def laad_muziek(data):
    geladen_afbeelding = {}
    for item in data:
        path = item['image']
        try:
            img = pygame.image.load(path).convert_alpha()
            geladen_afbeelding[path] = img
        except pygame.error:
            print(f"Kon {path} niet laden. Gebruik placeholder.")
            placeholder = pygame.Surface((CENTER_SIZE, CENTER_SIZE))
            placeholder.fill(BLAUW) 
            geladen_afbeelding[path] = placeholder

    return geladen_afbeelding

loaded_images = laad_muziek(music_spotify)

#load music menu
def teken_muziekspeler(surface, y, index, data, geladen_afbeelding, current_status, indexmusic):
    global scroll_offset
    center_x = 1000

    header = font_medium.render("MUSIC", True, WIT)
    header_rect = header.get_rect(center=(SCREEN_WIDTH - 150, 90))
    surface.blit(header, header_rect)

    keuze_music = font_klein.render(music_keuze[indexmusic], True, WIT)
    keuze_music_rect = keuze_music.get_rect(center=(SCREEN_WIDTH - 150, SCREEN_HEIGHT // 2 - 170))
    surface.blit(keuze_music, keuze_music_rect)

    pijl_links = font_medium.render("←", True, WIT)
    pijl_rechts = font_medium.render("→", True, WIT)
    surface.blit(pijl_links, (SCREEN_WIDTH - 270, SCREEN_HEIGHT // 2 - 190))
    surface.blit(pijl_rechts, (SCREEN_WIDTH - 50, SCREEN_HEIGHT // 2 - 190))

    if indexmusic == 0 or indexmusic == 2 or indexmusic == 3:
        for offset in range(-1, 2):
            draw_index = index + offset

            if 0 <= draw_index < len(data):
                item = data[draw_index]
                original_img = geladen_afbeelding[item['image']]

                if offset == 0:
                    img = pygame.transform.scale(original_img, (CENTER_SIZE, CENTER_SIZE))
                    img_rect = img.get_rect(center=(center_x, ALBUM_Y + CENTER_SIZE // 2))
                    alpha = 255
                else:
                    img = pygame.transform.scale(original_img, (SIDE_SIZE, SIDE_SIZE))
                    alpha = 80 # Lager alfa = donkerder/transparanter
                    img.set_alpha(alpha)

                    if offset == 1:
                        img_rect = img.get_rect(right=center_x - CENTER_SIZE // 2 - 20, 
                                                centery=ALBUM_Y + CENTER_SIZE // 2)
                    else: # Rechts
                        img_rect = img.get_rect(left=center_x + CENTER_SIZE // 2 + 20, 
                                                centery=ALBUM_Y + CENTER_SIZE // 2)
                
                surface.blit(img, img_rect)

        huidig_album = data[index]
        titel_tekst = font_medium.render(huidig_album['title'], True, WIT)
        titel_rect = titel_tekst.get_rect(center=(center_x, ALBUM_Y + CENTER_SIZE + 50))
        surface.blit(titel_tekst, titel_rect)
        
        artiest_tekst = font_klein.render(huidig_album['artist'], True, DONKER_GRIJS)
        artiest_rect = artiest_tekst.get_rect(center=(center_x, ALBUM_Y + CENTER_SIZE + 95))
        surface.blit(artiest_tekst, artiest_rect)
        
    if indexmusic == 1:
        if music_index >= scroll_offset + 3:
            scroll_offset = music_index - 3 + 1
        if music_index < scroll_offset:
            scroll_offset = music_index

        for i in range(3):
            item_index = scroll_offset + i
            
            # Stop if we get on last radio station
            if item_index >= len(music_radio):
                break
                
            tekst = music_radio[item_index]
            

            item_y = y + (i * 40)
            is_selected = (item_index == music_index)
            
            if is_selected:
                kleur = WIT
                label = font_medium.render(tekst, True, kleur)
            else:
                kleur = GRIJS_LICHT
                label = font_klein.render(tekst, True, kleur)
                
            surface.blit(label, (960, item_y + 10))

    if iconmusic == 0:
        try:
            playpause = pygame.image.load('pauze_icon.png').convert_alpha()
            playpause = pygame.transform.scale(playpause, (50,50))
        except pygame.error as e:
            print(f"Kon pauze_icon.png niet laden: {e}")
            playpause = None
    elif iconmusic == 1:
        try:
            playpause = pygame.image.load('play_icon.png').convert_alpha()
            playpause = pygame.transform.scale(playpause, (50,50))
        except pygame.error as e:
            print(f"Kon play_icon.png niet laden: {e}")
            playpause = None

    playpause_rect = playpause.get_rect()
    playpause_rect.center = (center_x - 20, SCREEN_HEIGHT - 200)
    surface.blit(playpause, playpause_rect)


#draw navigation menu    
def teken_scroll_lijst(surface, x, y, breedte, current_status, navig):
    global scroll_offset
    if nav_index >= scroll_offset + 3:
        scroll_offset = nav_index - 3 + 1
        
    if nav_index < scroll_offset:
        scroll_offset = nav_index

    nav_opties = nav_lijsten[navig]
        

    for i in range(3):
        item_index = scroll_offset + i
        
        # Stop if we get in last menu
        if item_index >= len(nav_opties):
            break
            
        tekst = nav_opties[item_index]

        item_y = y + (i * 40)

        is_selected = (item_index == nav_index)
        
        if is_selected:
            kleur = WIT
            label = font_medium.render(tekst, True, kleur)
        else:
            kleur = GRIJS_LICHT
            label = font_klein.render(tekst, True, kleur)

        surface.blit(label, (x + 10, item_y + 10))

    if current_status == "nav":
        label_tekst = font_medium.render("NAVIGATION", True, WIT)
        label_rect = label_tekst.get_rect(center=(SCREEN_WIDTH - 150, 90))
        surface.blit(label_tekst, label_rect)

        
        keuze_nav = font_klein.render(navi_keuze[navig], True, WIT)
        keuze_nav_rect = keuze_nav.get_rect(center=(SCREEN_WIDTH - 150, SCREEN_HEIGHT // 2 - 170))
        surface.blit(keuze_nav, keuze_nav_rect)

        pijl_links = font_medium.render("←", True, WIT)
        pijl_rechts = font_medium.render("→", True, WIT)
        surface.blit(pijl_links, (SCREEN_WIDTH - 270, SCREEN_HEIGHT // 2 - 190))
        surface.blit(pijl_rechts, (SCREEN_WIDTH - 50, SCREEN_HEIGHT // 2 - 190))

    #When pressed, load images and text with information about the chosen destination
    if dest == 1:
        try:
            finish = pygame.image.load('finish.png').convert_alpha()
            finish = pygame.transform.scale(finish, (50,50))
        except pygame.error as e:
            print(f"Kon finish.png niet laden: {e}")
            ventilatie = None
        finish_rect = finish.get_rect()
        finish_rect.center = (100, SCREEN_HEIGHT - 100)
        surface.blit(finish, finish_rect)

        finishtekst = font_medium.render("20km - ETA 11:55 - 83%", True, BLAUW)
        stoptekst = font_klein.render("duwen om te annuleren", True, BLAUW)
        surface.blit(finishtekst, (150, SCREEN_HEIGHT - 110))
        surface.blit(stoptekst, (200, SCREEN_HEIGHT - 80))

        desttekst = font_klein.render(f"Navigatie naar {actieve_gps}", True, BLAUW)
        surface.blit(desttekst, (150, SCREEN_HEIGHT - 600))

#draw phone menu
def teken_scroll_lijst_phone(surface, x, y, breedte, current_status, indexphone):
    global scroll_offset
    
    if phone_index >= scroll_offset + 3:
        scroll_offset = phone_index - 3 + 1

    if phone_index < scroll_offset:
        scroll_offset = phone_index


    for i in range(3):
        item_index = scroll_offset + i

        phone_opties = phone_lijsten[indexphone]

        if item_index >= len(phone_opties):
            break
            
        tekst = phone_opties[item_index]

        item_y = y + (i * 40)

        is_selected = (item_index == phone_index)
        
        if is_selected:
            kleur = WIT
            label = font_medium.render(tekst, True, kleur)
        else:
            kleur = GRIJS_LICHT
            label = font_klein.render(tekst, True, kleur)
            
        surface.blit(label, (x + 10, item_y + 10))

    if current_status == "phone":
        label_tekst = font_medium.render("PHONE", True, WIT)
        label_rect = label_tekst.get_rect(center=(SCREEN_WIDTH - 150, 90))
        surface.blit(label_tekst, label_rect)

        keuze_phone = font_klein.render(phone_keuze[indexphone], True, WIT)
        keuze_phone_rect = keuze_phone.get_rect(center=(SCREEN_WIDTH - 150, SCREEN_HEIGHT // 2 - 170))
        surface.blit(keuze_phone, keuze_phone_rect)

        pijl_links = font_medium.render("←", True, WIT)
        pijl_rechts = font_medium.render("→", True, WIT)
        surface.blit(pijl_links, (SCREEN_WIDTH - 270, SCREEN_HEIGHT // 2 - 190))
        surface.blit(pijl_rechts, (SCREEN_WIDTH - 50, SCREEN_HEIGHT // 2 - 190))

#If dial is pressed, draw menu with name and icon on it
    if calling == 1:
        pygame.draw.rect(screen, GRIJS_LICHT, (SCREEN_WIDTH - 300, SCREEN_HEIGHT - 60, 280, 50))
        try:
            phone_icon = pygame.image.load('hangup.png').convert_alpha()
            phone_icon = pygame.transform.scale(phone_icon, (45,45))
        except pygame.error as e:
            print(f"Kon ruitventil.png niet laden: {e}")

        phone_icon_rect = phone_icon.get_rect()
        phone_icon_rect.center = (SCREEN_WIDTH - 275, SCREEN_HEIGHT - 35)
        surface.blit(phone_icon, phone_icon_rect)

        keuze_calling = font_medium.render(actieve_phone, True, WIT)
        keuze_calling_rect = keuze_calling.get_rect(center=(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 35))
        surface.blit(keuze_calling, keuze_calling_rect)
        pygame.draw.rect(screen, DONKER_GRIJS, (SCREEN_WIDTH - 30, SCREEN_HEIGHT - 60, 250, 50))

#This puts the self made icons on the HUD where you can see in which position you are with the dial       
def teken_status_icoon(surface, center_x, current_status):
    icoon_surface = None
    
    if current_status == "nav":
        icoon_surface = nav 
    elif current_status == "phone":
        icoon_surface = phone 
    elif current_status == "tempR":
        icoon_surface = tempr
    elif current_status == "tempL":
        icoon_surface = templ
    elif current_status == "music":
        icoon_surface = music

    if icoon_surface:
        pos_rect = icoon_surface.get_rect()
        pos_rect.center = (center_x + 300, 90)
        surface.blit(icoon_surface, pos_rect)


#draw climate menus
def teken_cirkel_meter(surface, center_x, center_y, radius, waardeL, waardeR, kleur, current_driver, eenheid, venti, ruit):  
    pygame.draw.circle(surface, MAGENTA, (center_x, center_y), radius, 5)

    eenheid_tekst = font_klein.render(eenheid, True, WIT)
    eenheid_rect = eenheid_tekst.get_rect(center=(center_x, center_y + 35))
    surface.blit(eenheid_tekst, eenheid_rect)
    

    if current_driver == "driver":
        label_tekst = font_medium.render("DRIVER", True, WIT)
        label_rect = label_tekst.get_rect(center=(center_x, center_y - radius + 50))
        surface.blit(label_tekst, label_rect)
        waarde_tekst = font_groot.render(f"{waardeL:.1f}", True, WIT)
        waarde_rect = waarde_tekst.get_rect(center=(center_x, center_y - 15))
        surface.blit(waarde_tekst, waarde_rect)
    elif current_driver == "passenger":
        label_tekst = font_medium.render("PASSENGER", True, WIT)
        label_rect = label_tekst.get_rect(center=(center_x, center_y - radius + 50))
        surface.blit(label_tekst, label_rect)
        waarde_tekst = font_groot.render(f"{waardeR:.1f}", True, WIT)
        waarde_rect = waarde_tekst.get_rect(center=(center_x, center_y - 15))
        surface.blit(waarde_tekst, waarde_rect)
    elif current_driver == "phone":
        label_tekst = font_medium.render("PHONE", True, WIT)
        label_rect = label_tekst.get_rect(center=(center_x, center_y - radius - 40))
        surface.blit(label_tekst, label_rect)
    
#ventilation submenus
    if venti == 1:
        KLEUR_auto = WIT
        KLEUR_soft = WIT_LICHT
        KLEUR_hard = WIT_LICHT
        pos = center_x
    elif venti == 0:
        KLEUR_auto = WIT_LICHT
        KLEUR_soft = WIT
        KLEUR_hard = WIT_LICHT
        pos = center_x - 75
    elif venti == 2:
        KLEUR_auto = WIT_LICHT
        KLEUR_soft = WIT_LICHT
        KLEUR_hard = WIT
        pos = center_x + 75
    
    try:
        ventilatie = pygame.image.load('ventilatie.png').convert_alpha()
        ventilatie = pygame.transform.scale(ventilatie, (50,50))
    except pygame.error as e:
        print(f"Kon ventilatie.png niet laden: {e}")
        ventilatie = None

    ventilatie_rect = ventilatie.get_rect()
    ventilatie_rect.center = (pos, SCREEN_HEIGHT - 480)
    surface.blit(ventilatie, ventilatie_rect)

    venti = font_klein.render("Medium", True, KLEUR_auto)
    venti_rect = venti.get_rect(center=(center_x, center_y - radius - 70))
    surface.blit(venti, venti_rect)

    venti_soft = font_klein.render("Soft", True, KLEUR_soft)
    venti_soft_rect = venti_soft.get_rect(center=(center_x - 75, center_y - radius - 70))
    surface.blit(venti_soft, venti_soft_rect)

    venti_hard = font_klein.render("Hard", True, KLEUR_hard)
    venti_hard_rect = venti_hard.get_rect(center=(center_x + 75, center_y - radius - 70))
    surface.blit(venti_hard, venti_hard_rect)

#draws a prototype of the navigation card
def teken_kaart_paneel(surface):
    blokken = [
        (100, 50, 40, 200), (140, 150, 30, 100), 
        (70, 350, 100, 80), (170, 350, 30, 100)
    ]
    
    pygame.draw.rect(surface, GRIJS_LICHT, (70, 50, 10, 500))
    pygame.draw.rect(surface, GRIJS_LICHT, (70, 250, 250, 10))
    
    for x, y, w, h in blokken:
        pygame.draw.rect(surface, DONKER_GRIJS, (x, y, w, h))

    route_punten = [(75, 50), (75, 255), (200, 255)]
    pygame.draw.lines(surface, BLAUW, False, route_punten, 10)
    
    # Nav-pijl
    pygame.draw.circle(surface, BLAUW, (100, 300), 15)
    pijl_tekst = font_klein.render("▲", True, WIT)
    surface.blit(pijl_tekst, (95, 290)) # Simpele pijl

    # car info
    bereik_tekst = font_klein.render(f"{DATA['accu_percentage']}%", True, GROEN_LICHT)
    bereik_km = font_klein.render(f"{DATA['bereik_km']} km", True, GROEN_LICHT)
    surface.blit(bereik_tekst, (10, SCREEN_HEIGHT - 60))
    surface.blit(bereik_km, (10, SCREEN_HEIGHT - 30))

#draws center information
def teken_midden_paneel(surface):
    global active_status, ruit, microvalue, ruitventil, hardpress
    center_x = SCREEN_WIDTH // 2
    
    snelheid_tekst = font_groot.render(f"{DATA['snelheid']}", True, WIT)
    eenheid_tekst = font_klein.render("km/h", True, WIT)
    
    snelheid_rect = snelheid_tekst.get_rect(center=(center_x, SCREEN_HEIGHT // 2 - 50))
    eenheid_rect = eenheid_tekst.get_rect(center=(center_x, SCREEN_HEIGHT // 2 + 5))
    
    surface.blit(snelheid_tekst, snelheid_rect)
    surface.blit(eenheid_tekst, eenheid_rect)

    try:
        snelheidsbord = pygame.image.load('maxsnel.png').convert_alpha()
        snelheidsbord = pygame.transform.scale(snelheidsbord, (75,75))
    except pygame.error as e:
        print(f"Kon snelheidsbord.png niet laden: {e}")
        snelheidsbord = None
        
    img_rect = snelheidsbord.get_rect()
    img_rect.center = (center_x, 100)
    surface.blit(snelheidsbord, img_rect)

    pijl_links = font_groot.render("←", True, GROEN_LICHT)
    pijl_rechts = font_groot.render("→", True, GROEN_LICHT)
    surface.blit(pijl_links, (center_x - 140, 140))
    surface.blit(pijl_rechts, (center_x + 70, 140))
    

    lijn_kleur = BLAUW
    lijn_dikte = 8
    
    # left lane
    pygame.draw.line(surface, lijn_kleur, (center_x - 180, SCREEN_HEIGHT), (center_x - 50, auto_rect.centery + 40), lijn_dikte)
    # right lane
    pygame.draw.line(surface, lijn_kleur, (center_x + 180, SCREEN_HEIGHT), (center_x + 50, auto_rect.centery + 40), lijn_dikte)
    
    balk_breedte = 250
    pygame.draw.rect(surface, GRIJS_LICHT, (50, SCREEN_HEIGHT - 40, balk_breedte, 5))
    pygame.draw.rect(surface, GROEN_LICHT, (50, SCREEN_HEIGHT - 40, int(balk_breedte * (DATA['accu_percentage']/100)), 5))

    #longpress logica for the different menus, saved in different variables
    try:
        hardpress = pygame.image.load('HardPress.png').convert_alpha()
        hardpress = pygame.transform.scale(hardpress, (90,70))
    except pygame.error as e:
        print(f"Kon HardPress.png niet laden: {e}")
    try:
        ruitventil = pygame.image.load('ruitventil.png').convert_alpha()
        ruitventil = pygame.transform.scale(ruitventil, (75,75))
    except pygame.error as e:
        print(f"Kon ruitventil.png niet laden: {e}") 
        

    alpha_waarde = 255 if ruit == 1 else 50
    alpha_waarde_2 = 255 if microvalue == 1 else 50
    
    if active_status in ["tempL", "tempR"]:
        rechter_icoon = ruitventil
        hardpress_temp = hardpress.copy()
        hardpress_temp.set_alpha(alpha_waarde)
        hardpress_rect = hardpress_temp.get_rect(center=(SCREEN_WIDTH - 190, SCREEN_HEIGHT - 75))
        surface.blit(hardpress_temp, hardpress_rect)
        icoon_temp = rechter_icoon.copy()
        icoon_temp.set_alpha(alpha_waarde)
        icoon_rect = icoon_temp.get_rect(center=(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 75))
        surface.blit(icoon_temp, icoon_rect)
    else:
        rechter_icoon = micro
        hardpress_temp = hardpress.copy()
        hardpress_temp.set_alpha(alpha_waarde_2)
        hardpress_rect = hardpress_temp.get_rect(center=(SCREEN_WIDTH - 190, SCREEN_HEIGHT - 75))
        surface.blit(hardpress_temp, hardpress_rect)
        icoon_temp = rechter_icoon.copy()
        icoon_temp.set_alpha(alpha_waarde_2)
        icoon_rect = icoon_temp.get_rect(center=(SCREEN_WIDTH - 100, SCREEN_HEIGHT - 75))
        surface.blit(icoon_temp, icoon_rect)


running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    if ser and ser.in_waiting > 0:
        try:
            data = ser.readline().decode('utf-8').strip() #load serial data from Arduino and decode to read it here
            if data == "nav":
                active_status = "nav"
                nav_index = 0
            elif data == "tempL":
                active_status = "tempL"
                active_driver = "driver"
            elif data == "tempR":
                active_status = "tempR"
                active_driver = "passenger"
            elif data == "music":
                active_status = "music"
                music_index = 0
                current_song = 0 # Reset
                if ser and ser.is_open:
                    if indexmusic in [0, 2, 3]:
                        ser.write("spotify\n".encode('utf-8')) #send information serial to Arduino for different haptic feedback functions
            elif data == "phone":
                active_status = "phone"
                active_driver = "phone"
                phone_index = 0
            elif data == "home":
                active_status = "home"
                
            elif data == "cw": #counterwise turning with dial, receive from Arduino
                nav_index +=1
                if active_status == "tempL":
                    DATA['temperatuurL'] = DATA['temperatuurL'] + 0.5
                    if DATA['temperatuurL'] > 26:
                        if ser and ser.is_open:
                            bericht = "limietcw"
                            ser.write(bericht.encode('utf-8'))
                        DATA['temperatuurL'] = 26
                elif active_status == "tempR":
                    DATA['temperatuurR'] = DATA['temperatuurR'] + 0.5
                    if DATA['temperatuurR'] > 26:
                        if ser and ser.is_open:
                            bericht = "limietcw"
                            ser.write(bericht.encode('utf-8'))
                        DATA['temperatuurR'] = 26
                elif active_status == "nav":
                    huidige_lijst = nav_lijsten[navig]
                    if navig == 1 or navig == 2:
                        if nav_index > len(huidige_lijst) - 1:
                            if ser and ser.is_open:
                                bericht = "limietcw"
                                ser.write(bericht.encode('utf-8'))
                            nav_index = len(huidige_lijst) - 1
                    else:
                        if nav_index >= len(huidige_lijst):
                            nav_index = 0
                elif active_status == "music":
                    huidige_lijst = music_lijsten[indexmusic]
                    if indexmusic == 1: # RADIO MODUS
                        music_index += 1
                        if music_index >= len(huidige_lijst):
                            music_index = 0
                    else: # SPOTIFY, BT, USB
                        current_song -= 1
                        if current_song < 0:
                            current_song = len(huidige_lijst) - 1
                elif active_status == "phone":
                    phone_index += 1
                    huidige_lijst = phone_lijsten[indexphone]
                    if phone_index >= len(huidige_lijst):
                        phone_index = 0
                        
                        
                
            elif data == "ccw": #counterclockwise  turning with dial, receive from Arduino
                nav_index -= 1
                phone_index -= 1
                if active_status == "tempL":
                    DATA['temperatuurL'] = DATA['temperatuurL'] - 0.5
                    current_song = min(len(music_spotify) -1, current_song + 1)
                    selected_index -= 1
                    if DATA['temperatuurL'] < 15:
                        if ser and ser.is_open:
                            bericht = "limietccw"
                            ser.write(bericht.encode('utf-8'))
                        DATA['temperatuurL'] = 15
                elif active_status == "tempR":
                    DATA['temperatuurR'] = DATA['temperatuurR'] - 0.5
                    current_song = min(len(music_spotify) -1, current_song + 1)
                    selected_index -= 1
                    if DATA['temperatuurR'] < 15:
                        if ser and ser.is_open:
                            bericht = "limietccw"
                            ser.write(bericht.encode('utf-8'))
                        DATA['temperatuurR'] = 15
                elif active_status == "music":
                    huidige_lijst = music_lijsten[indexmusic]
                    if indexmusic == 1: # RADIO MODUS
                        music_index -= 1
                        if music_index < 0:
                            music_index = len(huidige_lijst) - 1
                    else: # SPOTIFY, BT, USB
                        current_song += 1
                        if current_song < 0:
                            current_song = len(huidige_lijst) - 1
                elif active_status == "nav":
                    huidige_lijst = nav_lijsten[navig]
                    if navig == 1 or navig == 2:
                        if nav_index < 0:
                            if ser and ser.is_open:
                                bericht = "limietccw"
                                ser.write(bericht.encode('utf-8'))
                            nav_index = 0
                    else:
                        if nav_index < 0:
                            nav_index = len(huidige_lijst) - 1
                elif active_status == "phone":
                    huidige_lijst = phone_lijsten[indexphone]
                    if phone_index < 0:
                        phone_index = len(huidige_lijst) - 1


                        
            elif data == "press": #press with dial, receive from Arduino
                if active_status == "tempL":
                    if balk_statusL < 3:
                        balk_statusL += 1
                    else:
                        balk_statusL = 0
                elif active_status == "tempR":
                    if balk_statusR < 3:
                        balk_statusR += 1
                    else:
                        balk_statusR = 0
                elif active_status == "music":
                    if ser and ser.is_open:
                        bericht = "limietcw"
                        ser.write(bericht.encode('utf-8'))
                    iconmusic += 1
                    if iconmusic > 1:
                        iconmusic = 0
                elif active_status == "phone":
                    if calling == 0:
                        huidige_lijst = phone_lijsten[indexphone]
                        actieve_phone = huidige_lijst[phone_index]
                        calling = 1
                        start_calling = pygame.time.get_ticks()
                    else:
                        calling = 0
                        calling_timer = 0
                        actieve_phone = ""
                elif active_status == "nav":
                    if dest == 0:
                        huidige_lijst = nav_lijsten[navig]
                        actieve_gps = huidige_lijst[nav_index]
                        dest = 1
                    else:
                        dest = 0
                        actieve_gps = ""

                    
            elif data == "longpress": #if dial is pressed +2seconds, Arduino sends longpress to pygame
                if active_status in ["tempL", "tempR"]:
                    if ruit == 0:
                        ruit = 1                            
                    else:
                        ruit = 0
                else:
                    microvalue = 1
                    microvalue_start_time = pygame.time.get_ticks()
                        

                    
            elif data == "left": #left button on dial to go through submenus
                selected_index = 0
                nav_index = 0
                phone_index = 0
                music_index = 0
                if ventil > 0:
                    ventil -= 1
                if navig > 0:
                    navig -= 1
                if indexphone > 0:
                    indexphone -= 1
                if active_status == "music":
                    if indexmusic > 0:
                        indexmusic -= 1
                        current_song = 0
                        if ser and ser.is_open:
                            if indexmusic in [0, 2, 3]:
                                ser.write("spotify\n".encode('utf-8'))
                    
            elif data == "right": #right button on dial to go through submenus
                selected_index = 0
                nav_index = 0
                phone_index = 0
                music_index = 0
                if ventil < 2:
                    ventil += 1
                if navig < 3:
                    navig += 1
                if indexphone < 2:
                    indexphone += 1
                if active_status == "music":
                    if indexmusic < 3:
                        indexmusic += 1
                        current_song = 0 # Reset song index bij bronwissel
                        if ser and ser.is_open:
                            if indexmusic in [0, 2, 3]:
                                ser.write("spotify\n".encode('utf-8'))             
                
        except Exception as e:
            print(f"Fout bij verwerken data: {e}")
                
            

    # draw background
    screen.fill(DONKER_GRIJS) 
    
    # Bovenste statusbalk (Weer & Tijd)
    weer_tekst = font_klein.render(" 15°C  |  21:23", True, WIT)
    screen.blit(weer_tekst, (20, 10))

    teken_kaart_paneel(screen)

    teken_cirkel_meter(
        screen, 
        SCREEN_WIDTH - 150, 
        SCREEN_HEIGHT // 2, 
        120, 
        DATA['temperatuurL'],
        DATA['temperatuurR'],
        MAGENTA,
        active_driver,
        "°C",
        ventil,
        ruit
    )

    center_x = SCREEN_WIDTH // 2
    teken_status_icoon(screen, center_x, active_status)

    
    weergave_status = 0
    if active_status == "tempL":
        weergave_status = balk_statusL
    elif active_status == "tempR":
        weergave_status = balk_statusR

#seat warming variables per driver
    for i in range(3):
        x = (center_x + 475) + i * 20 
        if (i + 1) <= weergave_status:
            kleur = ROOD if active_status == "tempR" else ROOD
        else:
            kleur = (100, 100, 100) 

        pygame.draw.rect(screen, kleur, (x, SCREEN_HEIGHT - 175, 10, 50))


    if active_status == "nav":
        pygame.draw.rect(screen, DONKER_GRIJS, vierkant) 

        teken_scroll_lijst(screen, 950,200,400, active_status, navig)
    elif active_status == "phone":
        pygame.draw.rect(screen, DONKER_GRIJS, vierkant) 
        teken_scroll_lijst_phone(screen, 950,200,400, active_status, indexphone)
        if calling == 1:
            milliseconden = pygame.time.get_ticks() - start_calling
            calling_timer = milliseconden // 1000
            
            minuten = calling_timer // 60
            seconden = calling_timer % 60
            tijd_string = f"{minuten:02}:{seconden:02}"

            timer_oppervlak = font_klein.render(tijd_string, True, WIT)
            screen.blit(timer_oppervlak, (SCREEN_WIDTH - 240, SCREEN_HEIGHT - 45))
    elif active_status == "music":
        pygame.draw.rect(screen, DONKER_GRIJS, vierkant)
        huidige_data = music_lijsten[indexmusic]
        actuele_index = music_index if indexmusic == 1 else current_song
        if actuele_index >= len(huidige_data):
            actuele_index = 0
            if indexmusic == 1: music_index = 0 
            else: current_song = 0
        teken_muziekspeler(screen, 200, actuele_index, huidige_data, loaded_images, active_status, indexmusic)
    elif active_status == "home":
        pygame.draw.rect(screen, DONKER_GRIJS, vierkant)

    if active_status in ["tempL", "tempR"]:
        stoel_rect = stoel.get_rect()
        climate_tekst = font_medium.render("CLIMATE", True, WIT)
        climate_tekst_rect = climate_tekst.get_rect(center=(SCREEN_WIDTH - 150, 90))
        screen.blit(climate_tekst, climate_tekst_rect)
        stoel_rect = stoel.get_rect()
        stoel_rect.center = center=(SCREEN_WIDTH - 165, SCREEN_HEIGHT - 150)
        screen.blit(stoel, stoel_rect)

    
    if microvalue == 1: 
        huidige_tijd = pygame.time.get_ticks()
        if huidige_tijd - microvalue_start_time > 5000: # 5000 milliseconden = 5 seconden
            microvalue = 0
            
    teken_midden_paneel(screen)
    
    #Volgende twee lijntjes aanzetten als HUD moet gespiegeld worden voor auto demonstratie:
    #canvas = pygame.transform.flip(screen, False, True)
    #screen.blit(canvas, (0,0))
    
    pygame.display.flip()
    
    clock.tick(60)


pygame.quit()
sys.exit()
```

