#pragma once

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {


class TestElement : public Node {
	GDCLASS(TestElement, Node)

protected:
    // This is required to expose methods, properties, and signals to Godot
    static void _bind_methods(); 

public:
    TestElement();
    ~TestElement();

    // Your custom function declaration
    static void TestMethod();
	
};


class GDExample : public Sprite2D {
	GDCLASS(GDExample, Sprite2D)

private:
	double time_passed;

protected:
	static void _bind_methods();

public:
	GDExample();
	~GDExample();

	void _process(double delta) override;
};

} // namespace godot}
