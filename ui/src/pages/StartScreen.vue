<script setup>
defineProps({
  typedStartTitle: {
    type: String,
    default: "",
  },
});
</script>

<template>
  <section class="splash-only">
    <h1 class="splash-title splash-typewriter">{{ typedStartTitle }}</h1>
  </section>
</template>
