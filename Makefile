# Makefile pour un unique ex�cutable

# liste des variables � renseigner
#   CIBLE : nom du programme ( $(CIBLE).c doit contenir main() )
#   SRCS : ensemble des fichiers sources 
#   LIBS : liste des biblioth�ques utiles � l'�dition des liens 
#          (format : -lnom1 -lnom2 ...) 
#   PREFIX : chemin de la hi�rarchie 
#
# NE PAS OUBLIER D'AJOUTER LA LISTE DES DEPENDANCES A LA FIN DU FICHIER

CIBLE = tp
SRCS =  src/shader.cpp src/objloader.cpp tp.cpp
LIBS =  -lglut -lGLU -lGL -lm -lGLEW -lpthread

#########################################################"

INCLUDES = . -I./third_party/glm-0.9.7.1
INCDIR = . -I./third_party/glm-0.9.7.1
LIBDIR = .
BINDIR = .

# nom du compilateur
CC = g++
CPP = g++

# options du compilateur          
CFLAGS = -Wall -O3 
CXXFLAGS = -Wall -O3 

# option du preprocesseur
CPPFLAGS =  -I$(INCDIR) 

# options du linker et liste des biblioth�ques � charger
LDFLAGS = -L/usr/X11R6/lib              
LDLIBS = -L$(LIBDIR) $(LIBS)  

# construire la liste des fichiers objets une nouvelle chaine � partir
# de SRCS en substituant les occurences de ".c" par ".o" 
OBJS = $(SRCS:.cpp=.o)   

# cible par d�faut
$(CIBLE): $(OBJS)

install:  $(CIBLE)
	cp $(CIBLE) $(BINDIR)/

installdirs:
	test -d $(INCDIR) || mkdir $(INCDIR)
	test -d $(LIBDIR) || mkdir $(LIBDIR)
	test -d $(BINDIR) || mkdir $(BINDIR)

clean:
	rm -f  *~  $(CIBLE) $(OBJS)

veryclean: clean
	rm -f $(BINDIR)/$(CIBLE)

dep:
	gcc $(CPPFLAGS) -MM $(SRCS)

# liste des d�pendances g�n�r�e par 'make dep'
shader.o: src/shader.cpp src/shader.hpp
objloader.o: src/objloader.cpp src/objloader.hpp
tp.o: tp.cpp src/shader.hpp src/objloader.hpp

