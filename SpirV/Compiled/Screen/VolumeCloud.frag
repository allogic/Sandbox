#   
  �                GLSL.std.450                     main    �  �  �               �       main         pn(vf3;   
   x        fpn(vf3;         p        rand(vf2;        co       SpiralNoiseC(vf3;        p        SpiralNoise3D(vf3;       p        Clouds(vf3;      p         map(vf3;         p     #   normalizer    %   p     (   f     5   uv    E   rg    I   uNoise    h   param     n   param     u   param     �   n     �   iter      �   i     �   n     �   iter      �   i       final       d0      param       d1    &  param     (  d2    8  param     :  d3    J  param     _  param     j  ProjectionUniform     j      uProjection   j     uView     j     uTransform    l        u  CameraUniform     u      uCameraPosition   u     uCameraRotation   u     uCameraLocalRight     u     uCameraLocalUp    u     uCameraLocalFront     w        �  param     �  GlobalUniform     �      uTime     �     uTimeDelta    �     uScreenSize   �        �  param     �  rd    �  gl_FragCoord      �  ro    �  ld    �  td    �  d     �  t     �  sundir    �  sun   �  col   �  bgcol     �  sum   �  pos1      �  seed      �  param     �  i     �  pos     param     0  param     7  w     ?  lin   J  col   �  param     �  oColor    �  VertOut   �      uv    �  fragIn  G  I   "       G  I   !       H  j         H  j      #       H  j            H  j        H  j     #   @   H  j           H  j        H  j     #   �   H  j           G  j     G  l  "       G  l  !      H  u      #       H  u     #      H  u     #       H  u     #   0   H  u     #   @   G  u     G  w  "       G  w  !      H  �      #       H  �     #      H  �     #      G  �     G  �  "       G  �  !       G  �        G  �         G  �     G  �              !                                        !  	                                !              "         ;  "   #      +     $   ��M?+     .     @@+     /      @+     8     B+     9     �A,     :   8   9     ;           +  ;   <         =          	 F                              G   F      H       G   ;  H   I       +     L      ?+     O     �C+     R         S         +     V     ��+     W   ��@+  ;   X       +  ;   [      +     f   H�z=+     l      >+     p     �>+     |   m��=+     ~   9�OA+        Lw�B,     �   ~      +     �   ��*G+     �     �?  �             �      �   +  �   �       +  �   �        �   +     �   �P=?+     �   ���?+  �   �      +  �   �      +       �-�?+       ��L>+        �2D+       
�#=+     #  �Q9+     .  w��=+     5   ��C+     @  B`�<+     G    ED+     M    �@+     R     A+     W     B+     b     A  i  S        j  i  i  i     k     j  ;  k  l        m     i    u                    v     u  ;  v  w        x        +     �    HB  �              �     �  ;  �  �        �        +     �    �@   �     S   ;  �  �     +  �   �        �        +     �  ��+     �  ��>+     �  �?,     �  �  �  �  +     �  ��?+     �  ��5?+     �    @?,     �  �  �  �  ,     �  �   L   �   +     �  ���=+     �  ���=+     �  
ף<,     �    �  �     �     S   ,  S   �  R   R   R   R   ,     �  �   �   +  �   �  @   +     �  ��|?+       RI:+         �B+  ;        +       �p}?+       )\�=+        ��̾+     #  ���>+     '  ���=+     ,  ���>+     @  �QX?+     A  �Mb?+     B  ��h?,     C  @  A  B  +     D  33�>+     E  ��>,     F  D  p   E  +     K    �?+     L    �?+     M    �?,     N  K  L  M  +     O  
�C?,     P  O  O  O  +     c  �ѹ+     �  �j<+     �  )\�>+     �  \�?+     �  333?,     �  #  �  �  ,     �  '    �     �     S   ;  �  �       �        �     �  ;  �  �     6               �     ;     �     ;     �     ;  =   �     ;  =   �     ;  =   �     ;  =   �     ;     �     ;  =   �     ;     �     ;     �     ;  �  �     ;     �     ;     �     ;     �     ;  �   �     ;     �     ;          ;     0     ;  =   7     ;     ?     ;  �  J     ;     �     >  #   $   =  S   �  �  O     �  �  �         A  �  �  �  �  =     �  �  �     �  �  L   �     �  �  �  A  �  �  �  �  [   =     �  �  P     �  �  �  �     �  �  �  Q     �  �      Q     �  �     P     �  �  �  V        �     E   �  >  �  �  A  x  �  w  �   =     �  �  >  �  �  >  �  R   >  �  R   >  �  �   >  �  R   >  �  �  =     �  �  =     �  �  �     �  �  �       �     +   �  R   �   >  �  �  A  =   �  �  [   =     �  �  �     �  �    �     �  �  �  �     �  �  �  P     �  �  �  �  �     �  �  �  >  �  �  =     �  �       �        �  R  �     �  �  �  =     �  �  �     �  �  �  >  �  �  =     �  �  >  �  �  >  �  �  =  S   �  �  O     �  �  �         A  �  �  �  �  =     �  �  �     �  �  �  >  �  �  =     �  �  A  �  �  �  �   =     �  �       �     
   �  P     �  �  �  �     �  �  �  >  �  �  =     �  �  �     �  �  �  >  �  �  9     �     �  �     �    �  �     �  �   �  >  �  �  >  �  �   �  �  �  �  �  �  �      �  �  �  �  =  �   �  �  �  �   �  �  �  �  �  �  �  �  �  =     �  �  =     �  �  =     �  �  �     �  �  �  �     �  �  �  >  �  �  =     �  �  �  �      �  �  �  �        �        �        �    =       �  =       �  �           �  �         �    �    �  �   	     �      =     
  �  �  �     
    �  �     	    �  �       �        �        �    A  =     �    =         �  �         �    �    �  �             �        �        �    �  �  �    =       �  >      9             �           >  �    =       �       !     (        >  �  !  =     "  �  �  �   $  "  #  �  &      �  $  %  &  �  %  =     (  �  �     )  '  (  >  �  )  =     *  �  =     +  �  =     -  �  �     .  -  ,  �     /  +  .  >  0  /  9     1      0  �     2  *  1  �     3  2  �       4     +   3  R   �   =     5  �  �     6  5  4  >  �  6  =     8  �  �     9  �   8  =     :  �  �     ;  9  :  >  7  ;  =     <  7  =     =  �  �     >  =  <  >  �  >  =     G  �  �     H  F  G  �     I  C  H  >  ?  I  =     Q  �  P     R  Q  Q  Q       S     .   N  P  R  =     T  �       U     (   �  T  Q     V  S      Q     W  S     Q     X  S     P  S   Y  V  W  X  U  >  J  Y  =     Z  ?  =  S   [  J  O     \  [  [            �     ]  \  Z  =  S   ^  J  O 	 S   _  ^  ]              >  J  _  =  S   `  J  O     a  `  `            =     b  �  =     d  �  �     e  c  d  =     f  �  �     g  e  f       h        g  �     i  �   h  P     j  i  i  i       k     .   a  b  j  =  S   l  J  O 	 S   m  l  k              >  J  m  A  =   n  J    =     o  n  �     p  o  #  A  =   q  J    >  q  p  A  =   r  J    =     s  r  =  S   t  J  O     u  t  t            �     v  u  s  =  S   w  J  O 	 S   x  w  v              >  J  x  =  S   y  �  =  S   z  J  A  =   {  �    =     |  {  �     }  �   |  �  S   ~  z  }  �  S     y  ~  >  �    �  &  �  &  =     �  �  �     �  �  �  >  �  �  =     �  �       �     (   �    >  �  �  =     �  �       �        �  =     �  �  =  �   �  �  o     �  �  P     �  �  �  �     �  �  �  >  �  �  9     �     �  �     �  �  �  �     �  �   �  �     �  �  �  >  �  �  =     �  �  �     �  �  L   =     �  �  �     �  �  �  >  �  �  �  �  �  �  =  �   �  �  �  �   �  �  �   >  �  �  �  �  �  �  =  S   �  �  P  S   �  R   R   R   R   P  S   �  �   �   �   �     S   �     +   �  �  �  >  �  �  A  =   �  �  [   =     �  �  �     �  �    �     �  �  �  �     �  �  �  P     �  �  �  �  �     �  �  �  >  �  �  =     �  �  A  =   �  �    =     �  �  �     �  �   �  �     �  �  �  =  S   �  �  O     �  �  �            �     �  �  �  >  �  �  =     �  �       �        �  .   �     �  �  �  =     �  �  �     �  �  �  >  �  �  =     �  �  Q     �  �      Q     �  �     Q     �  �     P  S   �  �  �  �  �   >  �  �  �  8  6            	   7     
   �     ;     %      ;     (      ;     5      ;     E      =     &   
        '         &   >  %   '   =     )   
        *      
   )   >  (   *   =     +   (   =     ,   (   �     -   +   ,   =     0   (   �     1   0   /   P     2   .   .   .   �     3   2   1   �     4   -   3   >  (   4   =     6   %   O     7   6   6          A  =   >   %   <   =     ?   >   �     @   :   ?   �     A   7   @   =     B   (   O     C   B   B          �     D   A   C   >  5   D   =  G   J   I   =     K   5   P     M   L   L   �     N   K   M   P     P   O   O   �     Q   N   P   X  S   T   J   Q      R   O     U   T   T          >  E   U   A  =   Y   E   X   =     Z   Y   A  =   \   E   [   =     ]   \   A  =   ^   (   <   =     _   ^        `      .   Z   ]   _   �     a   W   `   �     b   V   a   �  b   8  6            	   7        �     ;     h      ;     n      ;     u      =     e      �     g   e   f   >  h   g   9     i      h   �     j   i   L   =     k      �     m   k   l   >  n   m   9     o      n   �     q   o   p   �     r   j   q   =     s      �     t   s   p   >  u   t   9     v      u   �     w   v   l   �     x   r   w   �  x   8  6               7        �     =     {      �     }   {   |   �     �   }   �        �         �   �     �   �   �        �      
   �   �  �   8  6            	   7        �     ;  =   �      ;  =   �      ;  �   �      >  �   R   >  �   �   >  �   �   �  �   �  �   �  �   �       �  �   �  �   =  �   �   �   �  �   �   �   �   �  �   �   �   �  �   A  =   �      [   =     �   �   =     �   �   �     �   �   �        �         �   A  =   �      X   =     �   �   =     �   �   �     �   �   �        �         �   �     �   �   �        �         �        �   �   =     �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �   A  =   �      [   =     �   �   A  =   �      X   =     �   �        �   �   P     �   �   �   �     �   �   �   =     �      O     �   �   �          �     �   �   �   =     �      O     �   �   �            >     �   =     �   #   =     �      O     �   �   �          �     �   �   �   =     �      O     �   �   �            >     �   A  =   �      <   =     �   �   A  =   �      X   =     �   �        �   �   P     �   �   �   �     �   �   �   =     �      O     �   �   �          �     �   �   �   =     �      O     �   �   �            >     �   =     �   #   =     �      O     �   �   �          �     �   �   �   =     �      O     �   �   �            >     �   =     �   �   �     �   �   �   >  �   �   �  �   �  �   =  �   �   �   �  �   �   �   �   >  �   �   �  �   �  �   =     �   �   �  �   8  6            	   7        �     ;  =   �      ;  =   �      ;  �   �      >  �   R   >  �   �   >  �   �   �  �   �  �   �  �   �       �  �   �  �   =  �   �   �   �  �   �   �   �   �  �   �   �   �  �   A  =   �      [   =     �   �   =     �   �   �     �   �   �        �         �   A  =   �      X   =     �   �   =     �   �   �     �   �   �        �         �   �     �   �   �   =     �   �   �     �   �   �   =     �   �   �     �   �   �   >  �   �   A  =   �      <   =     �   �   A  =   �      X   =     �   �        �   �   P     �   �   �   �     �   �   �   =     �      O     �   �   �          �     �   �   �   =     �      O     �   �   �            >     �   =     �   #   =     �      O     �   �   �          �        �   �   =          O                     >       =       �   �           >  �     �  �   �  �   =  �     �   �  �       �   >  �     �  �   �  �   =       �   �    8  6            	   7        �     ;  =        ;  =        ;          ;  =        ;     &     ;  =   (     ;     8     ;  =   :     ;     J     ;     _     A  =        [   =         >      =          O                     �           P             �           >      9            >      A  =        <   =         A  =        X   =         P         R     �           A  =         [   =     !     P     "  R   !  R   �     $  "  #  �     %    $  >  &  %  9     '     &  >    '  A  =   )     X   =     *  )  A  =   +     <   =     ,  +  P     -  *  R   ,  �     /  -  .  A  =   0     [   =     1  0  P     2  R   1  R   �     3  2  #  �     4  /  3  P     6  5  5  5  �     7  4  6  >  8  7  9     9     8  >  (  9  A  =   ;     X   =     <  ;  A  =   =     <   =     >  =  P     ?  <  R   >  �     A  ?  @  A  =   B     [   =     C  B  P     D  R   C  R   �     E  D  #  �     F  A  E  P     H  G  G  G  �     I  F  H  >  J  I  9     K     J  >  :  K  =     L    �     N  L  M  =     O    �     P  O  N  >    P  =     Q    �     S  Q  R  =     T    �     U  T  S  >    U  =     V  (  �     X  V  W  =     Y    �     Z  Y  X  >    Z  =     [  :  =     \    =     ]     �     ^  ]    >  _  ^  9     `     _       a     .   [  \  `  �     c  a  b  =     d    �     e  d  c  >    e  =     f    �  f  8  6             	   7        �  !   ;     �     ;     �     A  m  n  l  �   =  i  o  n  A  m  p  l  �   =  i  q  p    i  r     "   q  �  i  s  o  r  =     t     A  x  y  w  �   =     z  y  �     {  t  z  Q     |  {      Q     }  {     Q     ~  {     P  S     |  }  ~  �   �  S   �  s    Q     �  �      Q     �  �     Q     �  �     Q     �  �     P  S   �  �  �  �  �  O     �  �  �            >     �  =     �     >  �  �  9     �     �  =     �     �     �  �  �  A  �  �  �  �   =     �  �  �     �  �  �  P     �  �  �  �  �     �  �  �  >  �  �  9     �     �  �     �  �  �  �  �  8  