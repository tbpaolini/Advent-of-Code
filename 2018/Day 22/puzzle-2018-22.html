<h2>--- Day 22: Mode Maze ---</h2><p>This is it, your final stop: the year <span title="Yes, really: there is no year zero.">-483</span>. It's snowing and dark outside; the only light you can see is coming from a small cottage in the distance. You make your way there and knock on the door.</p>
<p>A portly man with a large, white beard answers the door and invites you inside. For someone living near the North Pole in -483, he must not get many visitors, but he doesn't act surprised to see you. Instead, he offers you some milk and cookies.</p>
<p>After talking for a while, he asks a favor of you. His friend hasn't come back in a few hours, and he's not sure where he is.  Scanning the region briefly, you discover one life signal in a cave system nearby; his friend must have taken shelter there.  The man asks if you can go there to retrieve his friend.</p>
<p>The cave is divided into square <em>regions</em> which are either dominantly <em>rocky</em>, <em>narrow</em>, or <em>wet</em> (called its <em>type</em>). Each region occupies exactly one <em>coordinate</em> in <code>X,Y</code> format where <code>X</code> and <code>Y</code> are integers and zero or greater. (Adjacent regions can be the same type.)</p>
<p>The scan (your puzzle input) is not very detailed: it only reveals the <em>depth</em> of the cave system and the <em>coordinates of the target</em>. However, it does not reveal the type of each region. The mouth of the cave is at <code>0,0</code>.</p>
<p>The man explains that due to the unusual geology in the area, there is a method to determine any region's type based on its <em>erosion level</em>. The erosion level of a region can be determined from its <em>geologic index</em>. The geologic index can be determined using the first rule that applies from the list below:</p>
<ul>
<li>The region at <code>0,0</code> (the mouth of the cave) has a geologic index of <code>0</code>.</li>
<li>The region at the coordinates of the target has a geologic index of <code>0</code>.</li>
<li>If the region's <code>Y</code> coordinate is <code>0</code>, the geologic index is its <code>X</code> coordinate times <code>16807</code>.</li>
<li>If the region's <code>X</code> coordinate is <code>0</code>, the geologic index is its <code>Y</code> coordinate times <code>48271</code>.</li>
<li>Otherwise, the region's geologic index is the result of multiplying the erosion <em>levels</em> of the regions at <code>X-1,Y</code> and <code>X,Y-1</code>.</li>
</ul>
<p>A region's <em>erosion level</em> is its <em>geologic index</em> plus the cave system's <em>depth</em>, all <a href="https://en.wikipedia.org/wiki/Modulo_operation">modulo</a> <code>20183</code>. Then:</p>
<ul>
<li>If the <em>erosion level modulo <code>3</code></em> is <code>0</code>, the region's type is <em>rocky</em>.</li>
<li>If the <em>erosion level modulo <code>3</code></em> is <code>1</code>, the region's type is <em>wet</em>.</li>
<li>If the <em>erosion level modulo <code>3</code></em> is <code>2</code>, the region's type is <em>narrow</em>.</li>
</ul>
<p>For example, suppose the cave system's depth is <code>510</code> and the target's coordinates are <code>10,10</code>. Using <code>%</code> to represent the modulo operator, the cavern would look as follows:</p>
<ul>
<li>At <code>0,0</code>, the geologic index is <code>0</code>. The erosion level is <code>(0 + 510) % 20183 = 510</code>. The type is <code>510 % 3 = 0</code>, <em>rocky</em>.</li>
<li>At <code>1,0</code>, because the <code>Y</code> coordinate is <code>0</code>, the geologic index is <code>1 * 16807 = 16807</code>. The erosion level is <code>(16807 + 510) % 20183 = 17317</code>. The type is <code>17317 % 3 = 1</code>, <em>wet</em>.</li> 
<li>At <code>0,1</code>, because the <code>X</code> coordinate is <code>0</code>, the geologic index is <code> 1 * 48271 = 48271</code>. The erosion level is <code>(48271 + 510) % 20183 = 8415</code>. The type is <code>8415 % 3 = 0</code>, <em>rocky</em>.</li>
<li>At <code>1,1</code>, neither coordinate is <code>0</code> and it is not the coordinate of the target, so the geologic index is the erosion level of <code>0,1</code> (<code>8415</code>) times the erosion level of <code>1,0</code> (<code>17317</code>), <code>8415 * 17317 = 145722555</code>. The erosion level is <code>(145722555 + 510) % 20183 = 1805</code>. The type is <code>1805 % 3 = 2</code>, <em>narrow</em>.</li>
<li>At <code>10,10</code>, because they are the target's coordinates, the geologic index is <code>0</code>. The erosion level is <code>(0 + 510) % 20183 = 510</code>. The type is <code>510 % 3 = 0</code>, <em>rocky</em>.</li>
</ul>
<p>Drawing this same cave system with rocky as <code>.</code>, wet as <code>=</code>, narrow as <code>|</code>, the mouth as <code>M</code>, the target as <code>T</code>, with <code>0,0</code> in the top-left corner, <code>X</code> increasing to the right, and <code>Y</code> increasing downward, the top-left corner of the map looks like this:</p>
<pre><code><em>M</em>=.|=.|.|=.|=|=.
.|=|=|||..|.=...
.==|....||=..|==
=.|....|.==.|==.
=|..==...=.|==..
=||.=.=||=|=..|=
|.=.===|||..=..|
|..==||=.|==|===
.=..===..=|.|||.
.======|||=|=.|=
.===|=|===<em>T</em>===||
=|||...|==..|=.|
=.=|=.=..=.||==|
||=|=...|==.=|==
|=.=||===.|||===
||.|==.|.|.||=||
</code></pre>
<p>Before you go in, you should determine the <em>risk level</em> of the area. For the rectangle that has a top-left corner of region <code>0,0</code> and a bottom-right corner of the region containing the target, add up the risk level of each individual region: <code>0</code> for rocky regions, <code>1</code> for wet regions, and <code>2</code> for narrow regions.</p>
<p>In the cave system above, because the mouth is at <code>0,0</code> and the target is at <code>10,10</code>, adding up the risk level of all regions with an <code>X</code> coordinate from <code>0</code> to <code>10</code> and a <code>Y</code> coordinate from <code>0</code> to <code>10</code>, this total is <code><em>114</em></code>.</p>
<p><em>What is the total risk level for the smallest rectangle that includes <code>0,0</code> and the target's coordinates?</em></p>
