import time
import math

n_secs = 20
fps = 30
n_frames = n_secs * fps

def scene_rotation(frame):
    # do one complete rotation in the time
    return (frame / n_frames) * 360

def absorber_rotation(frame):
    # do 9 complete rotations in the time
    return (frame / n_frames) * 360 * 9

def attenuation_length(frame):
    start_val = 70
    start_frame = 0
    middle_val = 1
    middle_frame = n_frames/2
    end_val = 70
    end_frame = n_frames

    if start_frame <= frame < middle_frame:
        t = frame/middle_frame
        a = start_val
        b = middle_val
    elif middle_frame <= frame:
        t = (frame-middle_frame)/(end_frame-middle_frame)
        a = middle_val
        b = end_val
    
    # blend between the two attenuation lengths in log space so that we always see something happening
    return math.exp(math.log(b) * t + math.log(a) * (1 - t))

for frame in range(n_frames):

    t = time.asctime()

    s_r = scene_rotation(frame)
    a_r = absorber_rotation(frame)
    l = attenuation_length(frame)

    with open("./scenes/portal/portal-crystal.pbrt.template") as template_file:
        template = template_file.read()
    template = template.replace("%%SCENE_ROTATION%%", str(s_r))
    template = template.replace("%%ABSORBER_ROTATION%%", str(a_r))
    template = template.replace("%%ATTENUATION%%", str(l))
    template = template.replace("%%FILENAME%%", f"anim_1_renders/portal-{frame:0>4}.png")
    with open(f"./anim_1_files/portal-{frame:0>4}.pbrt", "w") as out_file:
        out_file.write(template)
