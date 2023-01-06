export module math;

export namespace math {

int
add(int arg1, int arg2);

}

module :private;

namespace math {

int
add(int arg1, int arg2)
{
    return (arg1 + arg2);
}

} // namespace math