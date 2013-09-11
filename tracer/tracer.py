import sys, pygame
pygame.init()

SCALE = 100.0

image = pygame.image.load(sys.argv[1])
screen = pygame.display.set_mode(image.get_size())

points = []

while 1:
    for event in pygame.event.get():
        if event.type == pygame.QUIT: sys.exit()
        if event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
            print pygame.mouse.get_pos()
            points.append(pygame.mouse.get_pos())
        if event.type == pygame.KEYDOWN and event.key == pygame.K_u:
            if len(points) > 0:
                points.pop()
        if event.type == pygame.KEYDOWN and event.key == pygame.K_s:
            with open("data.h", "w") as f:
                f.write("static const GLfloat g_vertex_buffer_data[] = {\n")
                for x,y in points:
                    y = image.get_size()[1] - y # flip the y
                    x -= image.get_size()[0]/2.0
                    y -= image.get_size()[1]/2.0
                    x /= SCALE
                    y /= SCALE
                    f.write("%ff,%ff,0.0f,\n" % (x,y));
                f.write("};\n");
            print "Wrote data.h with %d points" % len(points)

    screen.blit(image, image.get_rect())

    for p in points:
        pygame.draw.circle(screen, (255,0,0), p, 5)

    if len(points) > 1:
        pygame.draw.lines(screen, (255,0,0), False, points, 2)

    pygame.display.flip()
